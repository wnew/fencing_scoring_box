//===========================================================================//
//                                                                           //
//  Desc:    Arduino Code to implement a fencing scoring apparatus           //
//  Dev:     Wnew                                                            //
//  Date:    Nov 2012                                                        //
//  Updated: July 2014                                                       //
//  Notes:   1. Basis of algorithm from digitalwestie on github. Thanks Mate //
//           2. Used uint8_t instead of int where possible to optimise       //
//           3. Lots of ADC optimisations to get faster ADC reads            //
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
#define TEST_ADC_SPEED
#define REPORT_TIMING

#define ADC_PRECISION 256


//============
// Pin Setup
//============
const uint8_t lamePinA     = 0;         // Lame A pin   - Analog
const uint8_t weaponPinA   = 1;         // Weapon A pin - Analog
//const int groundPinA   = 2;         // Ground A pin - Analog
//const int groundPinB   = 3;         // Ground B pin - Analog
const uint8_t weaponPinB   = 4;         // Weapon B pin - Analog
const uint8_t lamePinB     = 5;         // Lame B pin   - Analog
     
const uint8_t modePin      = 0;         // Mode change button interrupt pin 0 (digital pin 2)
const uint8_t buzzerPin    = 3;         // Pin to control the buzzer
const uint8_t modeLeds[]   = {4, 5, 6}; // LED pins to indicate weapon mode selected
const uint8_t irPin        = 13;        // IR receiver pin

const uint8_t onTargetA    = 7;         // On Target A Light
const uint8_t offTargetA   = 8;         // Off Target A Light
//const uint8_t shortLEDA    = 9;         // Short Circuit A Light
//const uint8_t shortLEDB    = 10;        // Short Circuit A Light
const uint8_t offTargetB   = 11;        // Off Target B Light
const uint8_t onTargetB    = 12;        // On Target B Light

uint8_t currentMode = 0;

// values of analog reads
int weaponA    = 0;
int weaponB    = 0;
int lameA      = 0;
int lameB      = 0;
//uint8_t groundA    = 0;
//uint8_t groundB    = 0;

long millisPastA     = 0;
long millisPastB     = 0;
long millisPastFirst = 0;

//==========================
// Lockout & Depress Times
//==========================
// the lockout time between hits for foil is 300ms +/-25ms
// the minimum amount of time the tip needs to be depressed for foil 14ms +/-1ms
// the lockout time between hits for epee is 45ms +/-5ms (40ms -> 50ms)
// the minimum amount of time the tip needs to be depressed for epee
// the lockout time between hits for sabre is 120ms +/-10ms
// the minimum amount of time the tip needs to be depressed for sabre 0.1ms -> 1ms
const int lockout [] = {300, 45, 120};
const int depress [] = { 14,  2,   1};

// mode constants
const uint8_t FOIL_MODE  = 0;
const uint8_t EPEE_MODE  = 1;
const uint8_t SABRE_MODE = 2;

bool hitA = false;
bool hitB = false;

bool isFirstHit = true;

// the threshold that the scoring triggers on (ADC_PRECISION/3)
int voltageThresh = ADC_PRECISION/3;

bool modeJustChangedFlag = false;

#ifdef TEST_ADC_SPEED
long now;
bool done = false;
long loopCount = 0;
#endif


//================
// Configuration
//================
void setup() {
   // set the internal pullup resistor on modePin
   pinMode(modePin, INPUT_PULLUP);
   // add the interrupt to the mode pin
   attachInterrupt(modePin, changeMode, RISING);
   pinMode(irPin,       INPUT);
   pinMode(buzzerPin,   OUTPUT);
   pinMode(modeLeds[0], OUTPUT);
   pinMode(modeLeds[1], OUTPUT);
   pinMode(modeLeds[2], OUTPUT);

   // set the light pins to outputs
   pinMode(offTargetA, OUTPUT);
   pinMode(offTargetB, OUTPUT);
   pinMode(onTargetA,  OUTPUT);
   pinMode(onTargetB,  OUTPUT);
   //pinMode(shortLEDA,  OUTPUT);
   //pinMode(shortLEDB,  OUTPUT);

   digitalWrite(modeLeds[currentMode], HIGH);

   // this optimises the ADC for our needs, see declaration for details
   adcOpt();

#ifdef DEBUG < 0
   Serial.begin(9600);
   Serial.println("3 Weapon Scoring Box");
   Serial.println("====================");
#endif

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
   bitClear(ADCSRA,ADPS0);
   bitClear(ADCSRA,ADPS1);
   bitSet  (ADCSRA,ADPS2);
}


//============
// Main Loop
//============
void loop() {
   // use a while as a main loop as the loop() has too much overhead for fast analogReads
   while(1) {
      checkIfModeChanged();

      weaponA = analogRead(weaponPinA);
      weaponB = analogRead(weaponPinB);
      lameA   = analogRead(lamePinA);
      lameB   = analogRead(lamePinB);
      signalHits();
      if (currentMode == FOIL_MODE)
         foil();
      if (currentMode == EPEE_MODE)
         epee();
      if (currentMode == SABRE_MODE)
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
   modeJustChangedFlag = true;
}

//============================
// Sets the correct mode led
//============================
void setModeLeds() {
   for (uint8_t i = 0; i < 3; i++) {
      digitalWrite(modeLeds[i], LOW);
   }
   digitalWrite(modeLeds[currentMode], HIGH);
}

//========================
// Run when mode changed
//========================
//TODO: Make this an interrupt
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
      Serial.print("Mode Changed to: ");
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
   // weapon A
   if (hitA == false) {
      // ignore if we've hit
      if (weaponA > voltageThresh) {
         if ((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + lockout[FOIL_MODE] > millis()))) {
            // if foil depress time has past we have a hit
            if (millis() <= (millisPastA + depress[FOIL_MODE])) {
               hitA = true;
               if (isFirstHit) {
                  millisPastFirst = millis();
               }
               // if other lame hit we have an onTarget
               if (lameB > voltageThresh) {
                  digitalWrite(onTargetA, HIGH);
                  digitalWrite(buzzerPin, HIGH);
#ifdef DEBUG < 0
                  Serial.println("A");
#endif
               // otherwise we have an offTarget
               } else {
                  digitalWrite(offTargetA, HIGH);
                  digitalWrite(buzzerPin,  HIGH);
#ifdef DEBUG < 0
                  Serial.println("C");
#endif
               }
            }
         }
      // nothing happening
      } else {
         millisPastA = millis();
      }
   }

   // weapon B
   if (hitB == false) {
      // ignore if we've hit
      if (weaponB > voltageThresh) {
         if ((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + lockout[FOIL_MODE] > millis()))) {
            // if foil depress time has past we have a hit
            if (millis() <= (millisPastB + depress[FOIL_MODE])) {
               hitB = true;
               if (isFirstHit) {
                  millisPastFirst = millis();
               }
               // if other lame hit we have an onTarget
               if (lameA > voltageThresh) {
                  digitalWrite(onTargetB, HIGH);
                  digitalWrite(buzzerPin, HIGH);
#ifdef DEBUG < 0
                  Serial.println("B");
#endif
               // otherwise we have an offTarget
               } else {
                  digitalWrite(offTargetB, HIGH);
                  digitalWrite(buzzerPin,  HIGH);
#ifdef DEBUG < 0
                  Serial.println("D");
#endif
               }
            }
         }
      // nothing happening
      } else {
         millisPastB = millis();
      }
   }
}

//===================
// Main epee method
//===================
void epee() {
   // weapon A
   if (hitA == false) {
      // ignore if we've hit
      if (weaponA < ADC_PRECISION - voltageThresh) {
         if ((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + lockout[EPEE_MODE] > millis()))) {
            // if epee depress time has past we have a hit
            if (millis() <= (millisPastA + depress[EPEE_MODE])) {
               // onTarget
               if (lameA > voltageThresh) {
                  if (isFirstHit) {
                     millisPastFirst = millisPastA;
                     isFirstHit = false;
                  }
                  digitalWrite(onTargetA, HIGH);
                  digitalWrite(buzzerPin, HIGH);
                  hitA = true;
#ifdef DEBUG < 0
                  Serial.println("A");
#endif
               // offTarget
               } else {
#ifdef DEBUG < 0
                  Serial.println("D");
#endif
               }
            }
         }
      // Nothing happening
      } else {
          millisPastA = millis();
      }
   }

   // weapon B
   if (hitB == false) {
      // ignore if we've hit
      if (weaponB < ADC_PRECISION - voltageThresh) {
         if ((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + lockout[EPEE_MODE] > millis()))) {
            // if epee depress time has past we have a hit
            if (millis() <= (millisPastB + depress[EPEE_MODE])) {
               // onTarget
               if (lameB > voltageThresh) {
                  if (isFirstHit) {
                     millisPastFirst = millisPastB;
                     isFirstHit = false;
                  }
                  digitalWrite(onTargetB, HIGH);
                  digitalWrite(buzzerPin, HIGH);
                  hitB = true;
#ifdef DEBUG < 0
                  Serial.println("B");
#endif
               // offTarget
               } else {
#ifdef DEBUG < 0
                  Serial.println("D");
#endif
               }
            }
         }
      // nothing happening
      } else {
         millisPastB = millis();
      }
   }
}

//====================
// Main sabre method
//====================
void sabre() {
   // weapon A
   if (hitA == false) {
      //ignore if we've hit
      if (weaponA > voltageThresh) {
         if ((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + lockout[SABRE_MODE] > millis()))) {
            // if sabre depress time has past we have a hit
            if (millis() <= (millisPastA + depress[SABRE_MODE])) {
               hitA = true;
               if (isFirstHit) {
                  millisPastFirst = millis();
               }
               //onTarget
               if (lameB > voltageThresh) {
                  digitalWrite(onTargetA, HIGH);
                  digitalWrite(buzzerPin, HIGH);
#ifdef DEBUG < 0
                  Serial.println("A");
#endif
               }
            }
         }
      // Nothing happening
      } else {
         millisPastA = millis();
      }
   }

   // weapon B
   if (hitB == false) {
      // ignore if we've hit
      if (weaponB > voltageThresh) {
         if ((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + lockout[SABRE_MODE] > millis()))) {
            // if sabre depress time has past we have a hit
            if (millis() <= (millisPastB + depress[SABRE_MODE])) {
               hitB = true;
               if (isFirstHit) {
                  millisPastFirst = millis();
               }
               // onTarget
               if (lameA > voltageThresh) {
                  digitalWrite(onTargetB, HIGH);
                  digitalWrite(buzzerPin, HIGH);
#ifdef DEBUG < 0
                  Serial.println("B");
#endif
               }
            }
         }
      // nothing happening
      } else {
         millisPastB = millis();
      }
   }
}

//===============
// Sets signals
//===============
void signalHits() {
   if (hitA || hitB) {
      // if lockout time is up
      if (millis() >= (millisPastFirst + lockout[currentMode])) {
         delay(1500);
         resetValues();
      }
   }
}

//===================
// Resets after hit
//===================
void resetValues() {
#ifdef DEBUG < 0
   Serial.println("R");
#endif
   digitalWrite(buzzerPin,  LOW);
   digitalWrite(onTargetA,  LOW);
   digitalWrite(offTargetA, LOW);
   digitalWrite(offTargetB, LOW);
   digitalWrite(onTargetB,  LOW);

   millisPastA = millis();
   millisPastB = millis();
   millisPastFirst = 0;

   hitA = false;
   hitB = false;

   isFirstHit = true;

   delay(100);
}