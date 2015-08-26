//===========================================================================//
//                                                                           //
//  Desc:    Arduino Code to implement a fencing scoring apparatus           //
//  Dev:     Wnew                                                            //
//  Date:    Nov 2012                                                        //
//  Updated: Aug 2014                                                        //
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
#define TEST_LIGHTS       // turns on lights for a second on start up
#define INT_PULL_UPS      // uses internal pullups for weapon pins
//#define TEST_ADC_SPEED  // used to test sample rate of ADCs
#define REPORT_TIMING     // prints timings over serial interface

//============
// Pin Setup
//============
const uint8_t shortLEDA  =  8;        // Short Circuit A Light
const uint8_t onTargetA  =  9;        // On Target A Light
const uint8_t offTargetA = 10;        // Off Target A Light
const uint8_t offTargetB = 11;        // Off Target B Light
const uint8_t onTargetB  = 12;        // On Target B Light
const uint8_t shortLEDB  = 13;        // Short Circuit A Light

const uint8_t weaponPinA = A0;        // Weapon A pin
const uint8_t weaponPinB = A1;        // Weapon B pin
const uint8_t lamePinA   = A2;        // Lame A pin (Epee return path)
const uint8_t lamePinB   = A3;        // Lame B pin (Epee return path)
const uint8_t groundPinA = A4;        // Ground A pin - Analog
const uint8_t groundPinB = A5;        // Ground B pin - Analog
     
const uint8_t modePin    = 0;         // Mode change button interrupt pin 0 (digital pin 2)
const uint8_t buzzerPin  = 3;         // Pin to control the buzzer
const uint8_t modeLeds[] = {4, 5, 6}; // LED pins to indicate weapon mode selected {f e s}

//=========================
// values of analog reads
//=========================
int weaponA    = 0;
int weaponB    = 0;
int lameA      = 0;
int lameB      = 0;
int groundA    = 0;
int groundB    = 0;

//======================
// depress and timouts
//======================
long microsPastA     = 0;
long microsPastB     = 0;
long microsPastFirst = 0;
bool isFirstHit      = true;

//==========================
// Lockout & Depress Times
//==========================
// the lockout time between hits for foil is 300ms +/-25ms
// the minimum amount of time the tip needs to be depressed for foil 14ms +/-1ms
// the lockout time between hits for epee is 45ms +/-5ms
// the minimum amount of time the tip needs to be depressed for epee 2ms
// the lockout time between hits for sabre is 120ms +/-10ms
// the minimum amount of time the tip needs to be depressed (in contact) for sabre 0.1ms -> 1ms
// These values are stored as micro seconds for more accuracy
//                         foil   epee   sabre
const long lockout [] = {300000, 45000, 120000};
const long depress [] = { 14000,  2000,    100};

//=================
// mode constants
//=================
const uint8_t FOIL_MODE  = 0;
const uint8_t EPEE_MODE  = 1;
const uint8_t SABRE_MODE = 2;

uint8_t currentMode = EPEE_MODE;

bool modeJustChangedFlag = false;

//=========
// states
//=========
boolean depressOnTargA  = false;
boolean depressOffTargA = false;
boolean depressOnTargB  = false;
boolean depressOffTargB = false;
boolean hitOnTargA      = false;
boolean hitOffTargA     = false;
boolean hitOnTargB      = false;
boolean hitOffTargB     = false;


#ifdef TEST_ADC_SPEED
long now;
long loopCount = 0;
bool done = false;
#endif


//================
// Configuration
//================
void setup() {
   // set the internal pullup resistor on modePin
   pinMode(modePin, INPUT_PULLUP);

   // add the interrupt to the mode pin
   attachInterrupt(modePin, changeMode, RISING);
   pinMode(buzzerPin,   OUTPUT);
   pinMode(modeLeds[0], OUTPUT);
   pinMode(modeLeds[1], OUTPUT);
   pinMode(modeLeds[2], OUTPUT);

   // set the light pins to outputs
   pinMode(offTargetA, OUTPUT);
   pinMode(offTargetB, OUTPUT);
   pinMode(onTargetA,  OUTPUT);
   pinMode(onTargetB,  OUTPUT);
   pinMode(shortLEDA,  OUTPUT);
   pinMode(shortLEDB,  OUTPUT);

   digitalWrite(modeLeds[currentMode], HIGH);

#ifdef INT_PULL_UPS
   // this turns on the internal pull up resistors for the weapon pins
   // think they are 20k resistors but need to check this
   // other pull ups in the circuit should be of the same value
   digitalWrite(A0, HIGH);
   digitalWrite(A1, HIGH);
#endif

#ifdef TEST_LIGHTS
   testLights();
#endif

   // this optimises the ADC to make the sampling rate quicker
   adcOpt();

   Serial.begin(57600);
   Serial.println("3 Weapon Scoring Box");
   Serial.println("====================");
   Serial.print  ("Mode : ");
   Serial.println(currentMode);

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
      checkIfModeChanged();
      // read analog pins
      weaponA = analogRead(weaponPinA);
      weaponB = analogRead(weaponPinB);
      lameA   = analogRead(lamePinA);
      lameB   = analogRead(lamePinB);
      signalHits();
      if      (currentMode == FOIL_MODE)
         foil();
      else if (currentMode == EPEE_MODE)
         epee();
      else if (currentMode == SABRE_MODE)
         sabre();

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

//=====================
// Mode pin interrupt
//=====================
void changeMode() {
   // set a flag to keep the time in the ISR to a min
   modeJustChangedFlag = true;
}

//============================
// Sets the correct mode led
//============================
void setModeLeds() {
   for (uint8_t i = 0; i < 3; i++) {
      digitalWrite(modeLeds[i], LOW);
   }
   // which is better?
   /*digitalWrite(modeLeds[0], LOW);
   digitalWrite(modeLeds[1], LOW);
   digitalWrite(modeLeds[2], LOW);*/
   digitalWrite(modeLeds[currentMode], HIGH);
}

//========================
// Run when mode changed
//========================
void checkIfModeChanged() {
 if (modeJustChangedFlag) {
      if (digitalRead(modePin)) {
         if (currentMode == 2)
            currentMode = 0;
         else
            currentMode++;
      }
      setModeLeds();
#ifdef DEBUG < 0
      Serial.print("Mode changed to: ");
      Serial.println(currentMode);
#endif
      //delay(200);
      modeJustChangedFlag = false;
   }
}


//===================
// Main foil method
//===================
void foil() {
   // this method is time critical, no serial comms or delay() here
   // also try keep all variables 8 bit
   // weapon A
   if (hitOnTargA == false && hitOffTargA == false) { // ignore if we've hit
      if (410 < weaponA && weaponA < 570 && lameB < 100) {
         if((isFirstHit == true) || ((isFirstHit == false) && (microsPastFirst + lockout[currentMode] > micros()))) {
            if  (micros() <= (microsPastA + depress[currentMode])) { // if 14ms or more have past we have a hit
               if(isFirstHit) {
                  microsPastFirst = micros();
               }
               // offTarget
               hitOffTargA = true;
            }
         }
      } else {
         if (100 < weaponA && weaponA < 410 && 100 < lameB && lameB < 410) {
            if((isFirstHit == true) || ((isFirstHit == false) && (microsPastFirst + lockout[currentMode] > micros()))) {
               if  (micros() <= (microsPastA + depress[currentMode])) { // if 14ms or more have past we have a hit
                  if(isFirstHit) {
                     microsPastFirst = micros();
                  }
                  // onTarget
                  hitOnTargA = true;
               }
            }
         } else { // nothing happening
            microsPastA = micros();
         }
      }
   }

   // weapon B
   if (hitOnTargB == false && hitOffTargB == false) { // ignore if we've hit
      if (410 < weaponB && weaponB < 570 && lameA < 100) {
         if((isFirstHit == true) || ((isFirstHit == false) && (microsPastFirst + lockout[currentMode] > micros()))) {
            if  (micros() <= (microsPastB + depress[currentMode])) { // if 14ms or more have past we have a hit
               if(isFirstHit) {
                  microsPastFirst = micros();
               }
               // offTarget
               hitOffTargB = true;
            }
         }
      } else {
         if (100 < weaponB && weaponB < 410 && 100 < lameA && lameA < 410) {
            if((isFirstHit == true) || ((isFirstHit == false) && (microsPastFirst + lockout[currentMode] > micros()))) {
               if  (micros() <= (microsPastB + depress[currentMode])) { // if 14ms or more have past we have a hit
                  if(isFirstHit) {
                     microsPastFirst = micros();
                  }
                  // onTarget
                  hitOnTargB = true;
               }
            }
         } else { // nothing happening
            microsPastB = micros();
         }
      }
   }
}

//===================
// Main epee method
//===================
void epee() {
   // this method is time critical, no serial comms or delay() here
   // also try keep all variables 8 bit
   // weapon A
   if (hitOnTargA == false) { // ignore if we've hit
      if (400 < weaponA && weaponA < 600 && lameB < 100) {
         if((isFirstHit == true) || ((isFirstHit == false) && (microsPastFirst + lockout[currentMode] > micros()))) {
            if  (micros() <= (microsPastA + depress[currentMode])) { // if 2ms or more have past we have a hit
               if(isFirstHit) {
                  microsPastFirst = micros();
               }
               // onTarget
               hitOnTargA = true;
            }
         }
      } else { // nothing happening
         microsPastA = micros();
      }
   }

   // weapon B
   if (hitOnTargB == false) { // ignore if we've hit
      if (400 < weaponB && weaponB < 600 && lameA < 100) {
         if((isFirstHit == true) || ((isFirstHit == false) && (microsPastFirst + lockout[currentMode] > micros()))) {
            if  (micros() <= (microsPastB + depress[currentMode])) { // if 2ms or more have past we have a hit
               if(isFirstHit) {
                  microsPastFirst = micros();
               }
               // onTarget
               hitOnTargB = true;
            }
         }
      } else { // nothing happening
         microsPastB = micros();
      }
   }
}

//====================
// Main sabre method
//====================
void sabre() {
   // this method is time critical, no serial comms or delay() here
   // also try keep all variables 8 bit
   // weapon A
   if (hitOnTargA == false && hitOffTargA == false) { // ignore if we've hit
      if (410 < weaponA && weaponA < 570 && lameB < 100) {
         if((isFirstHit == true) || ((isFirstHit == false) && ((microsPastFirst + lockout[currentMode]) > micros()))) {
            if  (micros() <= (microsPastA + depress[currentMode])) { // if 1ms or more have past we have a hit
               if(isFirstHit) {
                  microsPastFirst = micros();
               }
               // onTarget
               hitOnTargA = true;
            }
         }
      } else { // nothing happening
         microsPastA = micros();
      }
   }

   // weapon B
   if (hitOnTargB == false && hitOffTargB == false) { // ignore if we've hit
      if (410 < weaponB && weaponB < 570 && lameA < 100) {
         if((isFirstHit == true) || ((isFirstHit == false) && ((microsPastFirst + lockout[currentMode]) > micros()))) {
            if  (micros() <= (microsPastB + depress[currentMode])) { // if 1ms or more have past we have a hit
               if(isFirstHit) {
                  microsPastFirst = micros();
               }
               // onTarget
               hitOnTargB = true;
            }
         }
      } else { // nothing happening
         microsPastB = micros();
      }
   }
}

//==============
// Signal Hits
//==============
void signalHits() {
   // non time critical, this is run after a hit has been detected
   if (hitOnTargA || hitOffTargA || hitOffTargB || hitOnTargB) {
      // if lockout time is up
      if (micros() >= (microsPastFirst + lockout[currentMode])) {
         digitalWrite(onTargetA,  hitOnTargA);
         digitalWrite(offTargetA, hitOffTargA);
         digitalWrite(offTargetB, hitOffTargB);
         digitalWrite(onTargetB,  hitOnTargB);
         digitalWrite(buzzerPin,  HIGH);
         String serData = String("hitOnTargA  : ") + hitOnTargA  + "\n"
                               + "hitOffTargA : "  + hitOffTargA + "\n"
                               + "hitOffTargB : "  + hitOffTargB + "\n"
                               + "hitOnTargB  : "  + hitOnTargB  + "\n";
         Serial.println(serData);
         delay(1500);
         resetValues();
      }
   }
}

//======================
// Reset all variables
//======================
void resetValues() {
   digitalWrite(buzzerPin,  LOW);
   delay(2000);
   digitalWrite(onTargetA,  LOW);
   digitalWrite(offTargetA, LOW);
   digitalWrite(offTargetB, LOW);
   digitalWrite(onTargetB,  LOW);
   digitalWrite(shortLEDA,  LOW);
   digitalWrite(shortLEDB,  LOW);

   //microsPastA = micros();
   //microsPastB = micros();
   microsPastA     = 0;
   microsPastB     = 0;
   microsPastFirst = 0;

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
   digitalWrite(shortLEDA,  HIGH);
   digitalWrite(shortLEDB,  HIGH);
   delay(1000);
   resetValues();
}
