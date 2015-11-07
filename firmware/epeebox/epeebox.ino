//===========================================================================//
//                                                                           //
//  Desc:    Arduino Code to implement an epee fencing scoring apparatus     //
//  Dev:     Wnew                                                            //
//  Date:    Nov  2012                                                       //
//  Updated: Sept 2015                                                       //
//  Notes:   1. Basis of algorithm from digitalwestie on github. Thanks Mate //
//           2. Used uint8_t instead of int where possible to optimise       //
//           3. Set ADC prescaler to 16 faster ADC reads                     //
//                                                                           //
//  To do:   1. Could use shift reg on lights and mode LEDs to save pins     //
//           2. Use interrupts for buttons                                   //
//           3. Implement short circuit LEDs (already provision for it)      //
//           4. Set up debug levels correctly                                //
//                                                                           //
//===========================================================================//

//============
// #defines
//============
//TODO: set up debug levels correctly
#define DEBUG 0
//#define TEST_LIGHTS       // turns on lights for a second on start up
//#define TEST_ADC_SPEED    // used to test sample rate of ADCs
//#define REPORT_TIMING     // prints timings over serial interface
#define BUZZERTIME  1000  // length of time the buzzer is kept on after a hit (ms)
#define LIGHTTIME   3000  // length of time the lights are kept on after a hit (ms)
#define BAUDRATE   57600  // baudrate of the serial debug interface

//============
// Pin Setup
//============
const uint8_t shortLEDA  =  8;    // Short Circuit A Light
const uint8_t onTargetA  =  9;    // On Target A Light
const uint8_t offTargetA = 10;    // Off Target A Light
const uint8_t offTargetB = 11;    // Off Target B Light
const uint8_t onTargetB  = 12;    // On Target B Light
const uint8_t shortLEDB  = 13;    // Short Circuit A Light

const uint8_t groundPinA = A0;    // Ground A pin - Analog
const uint8_t lamePinA   = A1;    // Lame   A pin - Analog (Epee return path)
const uint8_t weaponPinA = A2;    // Weapon A pin - Analog
const uint8_t weaponPinB = A3;    // Weapon B pin - Analog
const uint8_t lamePinB   = A4;    // Lame   B pin - Analog (Epee return path)
const uint8_t groundPinB = A5;    // Ground B pin - Analog

const uint8_t buzzerPin  =  3;    // buzzer pin

//=========================
// values of analog reads
//=========================
int weaponA = 0;
int weaponB = 0;
int lameA   = 0;
int lameB   = 0;
int groundA = 0;
int groundB = 0;

//=======================
// depress and timeouts
//=======================
long depressAtime = 0;
long depressBtime = 0;
bool lockedOut    = false;

//==========================
// Lockout & Depress Times
//==========================
// the lockout time between hits for foil is 300ms +/-25ms
// the minimum amount of time the tip needs to be depressed for foil 14ms +/-1ms
// the lockout time between hits for epee is 45ms +/-5ms (40ms -> 50ms)
// the minimum amount of time the tip needs to be depressed for epee 2ms
// the lockout time between hits for sabre is 120ms +/-10ms
// the minimum amount of time the tip needs to be depressed for sabre 0.1ms -> 1ms
// These values are stored as micro seconds for more accuracy
//                         foil    epee   sabre
const long lockout [] = {300000,  45000, 120000};  // the lockout time between hits
const long depress [] = { 14000,   2000,   1000};  // the minimum amount of time the tip needs to be depressed

//=========
// states
//=========
boolean depressedA  = false;
boolean depressedB  = false;
boolean hitOnTargA  = false;
boolean hitOffTargA = false;
boolean hitOnTargB  = false;
boolean hitOffTargB = false;

#ifdef TEST_ADC_SPEED
long now;
long loopCount = 0;
bool done = false;
#endif


//================
// Configuration
//================
void setup() {

   // set the light pins to outputs
   pinMode(offTargetA, OUTPUT);
   pinMode(offTargetB, OUTPUT);
   pinMode(onTargetA,  OUTPUT);
   pinMode(onTargetB,  OUTPUT);
   pinMode(shortLEDA,  OUTPUT);
   pinMode(shortLEDB,  OUTPUT);
   pinMode(buzzerPin,  OUTPUT);

#ifdef TEST_LIGHTS
   testLights();
#endif

   // this optimises the ADC to make the sampling rate quicker
   //adcOpt();

   Serial.begin(BAUDRATE);
   Serial.println("Epee Scoring Box");
   Serial.println("================");

   resetValues();
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
   bitClear(ADCSRA, ADPS0);
   bitClear(ADCSRA, ADPS1);
   bitSet  (ADCSRA, ADPS2);
}


//============
// Main Loop
//============
void loop() {
   // use a while as a main loop as the loop() has too much overhead for fast analogReads
   // we get a 3-4% speed up on the loop this way
   while(1) {
      // read analog pins
      weaponA = analogRead(weaponPinA);
      weaponB = analogRead(weaponPinB);
      lameA   = analogRead(lamePinA);
      lameB   = analogRead(lamePinB);

      signalHits();
      epee();

#ifdef TEST_ADC_SPEED
      if (loopCount == 0) {
         now = micros();
      }
      loopCount++;
      if ((micros()-now >= 1000000) && done == false) {
         Serial.print(loopCount);
         Serial.println(" readings in 1 sec");
         done = true;
      }
#endif
   }
}


//===================
// Main epee method
//===================
void epee() {
   long now = micros();
   if ((hitOnTargA && (depressAtime + lockout[1] < now)) || (hitOnTargB && (depressBtime + lockout[1] < now))) {
      lockedOut = true;
   }

   // weapon A
   //  no hit for A yet    && weapon depress    && opponent lame touched
   if (hitOnTargA == false) {
      if (400 < weaponA && weaponA < 600 && 400 < lameA && lameA < 600) {
         if (!depressedA) {
            depressAtime = micros();
            depressedA   = true;
         } else {
            if (depressAtime + depress[1] <= micros()) {
               hitOnTargA = true;
            }
         }
      } else {
         // reset these values if the depress time is short.
         if (depressedA == true) {
            depressAtime = 0;
            depressedA   = 0;
         }
      }
   }

   // weapon B
   //  no hit for B yet    && weapon depress    && opponent lame touched
   if (hitOnTargB == false) {
      if (400 < weaponB && weaponB < 600 && 400 < lameB && lameB < 600) {
         if (!depressedB) {
            depressBtime = micros();
            depressedB   = true;
         } else {
            if (depressBtime + depress[1] <= micros()) {
               hitOnTargB = true;
            }
         }
      } else {
         // reset these values if the depress time is short.
         if (depressedB == true) {
            depressBtime = 0;
            depressedB   = 0;
         }
      }
   }
}


//==============
// Signal Hits
//==============
void signalHits() {
   // non time critical, this is run after a hit has been detected
   if (lockedOut) {
      digitalWrite(onTargetA,  hitOnTargA);
      digitalWrite(offTargetA, hitOffTargA);
      digitalWrite(offTargetB, hitOffTargB);
      digitalWrite(onTargetB,  hitOnTargB);
      digitalWrite(buzzerPin,  HIGH);
#ifdef DEBUG
      String serData = String("hitOnTargA  : ") + hitOnTargA  + "\n"
                            + "hitOffTargA : "  + hitOffTargA + "\n"
                            + "hitOffTargB : "  + hitOffTargB + "\n"
                            + "hitOnTargB  : "  + hitOnTargB  + "\n"
                            + "Locked Out  : "  + lockedOut   + "\n";
      Serial.println(serData);
#endif
      resetValues();
   }
}


//======================
// Reset all variables
//======================
void resetValues() {
   delay(BUZZERTIME);             // wait before turning off the buzzer
   digitalWrite(buzzerPin,  LOW);
   delay(LIGHTTIME-BUZZERTIME);   // wait before turning off the lights
   digitalWrite(onTargetA,  LOW);
   digitalWrite(offTargetA, LOW);
   digitalWrite(offTargetB, LOW);
   digitalWrite(onTargetB,  LOW);
   digitalWrite(shortLEDA,  LOW);
   digitalWrite(shortLEDB,  LOW);

   lockedOut    = false;
   depressAtime = 0;
   depressedA   = false;
   depressBtime = 0;
   depressedB   = false;

   hitOnTargA  = false;
   hitOffTargA = false;
   hitOnTargB  = false;
   hitOffTargB = false;

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
   digitalWrite(shortLEDA,  HIGH);
   digitalWrite(shortLEDB,  HIGH);
   delay(1000);
   resetValues();
}
