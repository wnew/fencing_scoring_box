//===========================================================================//
//                                                                           //
//  Desc:    Arduino Code to implement a sabre fencing scoring apparatus     //
//  Dev:     Wnew                                                            //
//  Date:    Nov 2012                                                        //
//  Updated: Aug 2014                                                       //
//  Notes:   1. Basis of algorithm from digitalwestie on github. Thanks Mate //
//           2. Used uint8_t instead of int where possible to optimise       //
//           3. Set ADC prescaler to 16 faster ADC reads                     //
//           4.                                                              //
//                                                                           //
//  To do:   1. Could use shift reg on lights and mode LEDs to save pins     //
//           2. Use interrupts for buttons                                   //
//           3. Implement short circuit LEDs                                 //
//           4. Set up debug levels correctly                                //
//                                                                           //
//===========================================================================//

//============
// #defines
//============
//TODO: set up debug levels correctly
#define DEBUG 0
#define TEST_LIGHTS
#define BUZZER

//============
// Pin Setup
//============
const uint8_t offTargetA = 10;        // Off Target A Light
const uint8_t offTargetB = 11;        // Off Target B Light
const uint8_t onTargetA  = 9;         // On Target A Light
const uint8_t onTargetB  = 12;        // On Target B Light

const uint8_t weaponPinA = 0;         // Weapon A pin
const uint8_t weaponPinB = 1;         // Weapon B pin
const uint8_t lamePinA   = 2;         // Lame A pin (Epee return path)
const uint8_t lamePinB   = 3;         // Lame B pin (Epee return path)

const uint8_t buzzerPin  = 4;

//=========================
// values of analog reads
//=========================
int weaponA    = 0;
int weaponB    = 0;
int lameA      = 0;
int lameB      = 0;

long millisPastA     = 0;
long millisPastB     = 0;
long millisPastFirst = 0;

//==========================
// Lockout & Depress Times
//==========================
int lockOut        = 300;    // the lockout time between hits for epee is 300ms
int minHitDuration = 14;     // the minimum amount of time the tip needs to be depressed

boolean hitOnTargA  = false;
boolean hitOffTargA = false;
boolean hitOnTargB  = false;
boolean hitOffTargB = false;

boolean isFirstHit = true;

int lowerThresh = 250;     // the threshold that the scoring triggers on (1024/4)
int midThresh   = 500;     // the threshold that the scoring triggers on (1024/4*2)
int upperThresh = 750;     // the threshold that the scoring triggers on (1024/4*3)


//================
// Configuration
//================
void setup() {
   pinMode(offTargetA, OUTPUT);
   pinMode(offTargetB, OUTPUT);
   pinMode(onTargetA,  OUTPUT);
   pinMode(onTargetB,  OUTPUT);

   pinMode(weaponPinA, INPUT);
   pinMode(weaponPinB, INPUT);
   pinMode(lamePinA,   INPUT);
   pinMode(lamePinB,   INPUT);

   pinMode(buzzerPin,  OUTPUT);

#ifdef TEST_LIGHTS
   testLights();
#endif

   adcOpt();

   Serial.begin(9600);
   Serial.print("Sabre Scoring Box\n");
   Serial.print("=================\n");
}

//=============
// ADC config
//=============
void adcOpt() {

   // the ADC only needs a couple of bits, the atmega is an 8 bit micro
   // so sampling only 8 bits makes the values easy/quicker to process
   // unfortunately this method only works on the Due.
   //analogReadResolution(8);

   // Data Input Disable Register
   // disconnects the digital inputs from which ever ADC channels you are using
   // an analog input will be float and cause the digital input to constantly
   // toggle high and low, this creates noise near the ADC, and uses extra 
   // power Secondly, the digital input and associated DIDR switch have a
   // capacitance associated with them which will slow down your input signal
   // if you’re sampling a highly resistive load 
   DIDR0 = 0x7F;

   // set the prescaler for the ADCs to 16 this allowes the fastest sampling
   bitClear(ADCSRA,ADPS0);
   bitClear(ADCSRA,ADPS1);
   bitSet  (ADCSRA,ADPS2);
}


//============
// Main Loop
//============
void loop() {
   weaponA = analogRead(weaponPinA);
   weaponB = analogRead(weaponPinB);
   lameA   = analogRead(lamePinA);
   lameB   = analogRead(lamePinB);
   //delay(1000);
   //Serial.println(weaponA);
   //Serial.println(weaponB);
   //Serial.println(lameA);
   //Serial.println(lameB);

   signalHits();

   // weapon A
   if (hitOnTargA == false && hitOffTargA == false) { // ignore if we've hit
      if (410 < weaponA && weaponA < 570 && lameB < 100) {
   	  	if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + lockOut > millis()))) {
            if  (millis() <= (millisPastA + minHitDuration)) { // if 14ms or more have past we have a hit
               if(isFirstHit) {
                  millisPastFirst = millis();
               }
               // onTarget
               hitOnTargA = true;
               digitalWrite(onTargetA, HIGH);
               digitalWrite(buzzerPin, HIGH);
            }
         }
      } else { // nothing happening
         millisPastA = millis();
      }
   }

   // weapon B
   if (hitOnTargB == false && hitOffTargB == false) { // ignore if we've hit
      if (410 < weaponB && weaponB < 570 && lameA < 100) {
   	  	if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + lockOut > millis()))) {
            if  (millis() <= (millisPastB + minHitDuration)) { // if 14ms or more have past we have a hit
               if(isFirstHit) {
                  millisPastFirst = millis();
               }
               // onTarget
               hitOnTargB = true;
               digitalWrite(onTargetB, HIGH);
               digitalWrite(buzzerPin, HIGH);
            }
         }
      } else { // nothing happening
         millisPastB = millis();
      }
   }
}

//=================
// Turn on buzzer
//=================
void signalHits() {
   if (hitOnTargA || hitOffTargA || hitOnTargB || hitOffTargB) {
      if (millis() >= (millisPastFirst + lockOut)) {
         // time for next action is up!
         delay(1000);
#ifdef BUZZER
         digitalWrite(buzzerPin, LOW);
#endif
         delay(2500);
         resetValues();
      }
   }
}

//======================
// Reset all variables
//======================
void resetValues() {
   Serial.println(hitOnTargA);
   Serial.println(hitOffTargA);
   Serial.println(hitOffTargB);
   Serial.println(hitOnTargB);

   digitalWrite(onTargetA,  LOW);
   digitalWrite(offTargetA, LOW);
   digitalWrite(offTargetB, LOW);
   digitalWrite(onTargetB,  LOW);

   //millisPastA = millis();
   //millisPastB = millis();
   millisPastA     = 0;
   millisPastB     = 0;
   millisPastFirst = 0;

   hitOnTargA  = false;
   hitOffTargA = false;
   hitOnTargB  = false;
   hitOffTargB = false;

   isFirstHit = true;

   delay(100);
}

//==============
// Test lights
//==============
void testLights() {
   digitalWrite(offTargetA, HIGH);
   digitalWrite(onTargetA,  HIGH);
   digitalWrite(offTargetB, HIGH);
   digitalWrite(onTargetB,  HIGH);
   delay(1000);
   resetValues();
}