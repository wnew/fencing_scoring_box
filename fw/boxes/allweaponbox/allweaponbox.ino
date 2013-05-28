//===========================================================================//
//                                                                           //
//  Desc: Arduino Code to implement a fencing scoring apparatus              //
//  Dev:  Wnew                                                               //
//  Date: Nov 2012                                                           //
//  Notes: Basis of code from digitalwestie on github                        //
//                                                                           //
//===========================================================================//

//============
// Pin Setup
//============
const int freePin0     = 0;    // Open analog pin
const int freePin1     = 1;    // Open analog pin
const int lamePinA     = 2;    // Lame A pin   - Analog
const int weaponPinA   = 3;    // Weapon A pin - Analog
const int groundPinA   = 4;    // Ground A pin - Analog
const int groundPinB   = 5;    // Ground B pin - Analog
const int weaponPinB   = 6;    // Weapon B pin - Analog
const int lamePinB     = 7;    // Lame B pin   - Analog

const int modePin      = 0;    // Mode change button interrupt pin 0 (digital pin 2)
const int buzzerPin    = 3;    // Pin to control the buzzer
const int modeLeds[]   = {4, 5, 6}; // LED pins to indicate weapon mode selected
const int irPin        = 13;    // IR receiver pin

const int onTargetA    = 7;    // On Target A Light
const int offTargetA   = 8;    // Off Target A Light
const int shortLEDA    = 9;    // Short Circuit A Light
const int shortLEDB    = 10;   // Short Circuit A Light
const int offTargetB   = 11;   // Off Target B Light
const int onTargetB    = 12;   // On Target B Light

int currentMode = 0;

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
// the lockout time between hits for foil is 300ms +/-25ms
// the minimum amount of time the tip needs to be depressed for foil 14ms +/-1ms
// the lockout time between hits for epee is 45ms +/-5ms (40ms -> 50ms)
// the minimum amount of time the tip needs to be depressed for epee
// the lockout time between hits for sabre is 120ms +/-10ms
// the minimum amount of time the tip needs to be depressed for sabre 0.1ms -> 1ms
const int lockout [] = {300, 45, 120};
const int depress [] = { 14,  2,   1};

boolean hitA = false;
boolean hitB = false;

boolean isFirstHit = true;

int voltageThresh = 340;     // the threshold that the scoring triggers on (1024/3)

unsigned long bit;
int lastTimeIRChecked;

// mode constants
const int FOIL_MODE  = 0;
const int EPEE_MODE  = 1;
const int SABRE_MODE = 2;

int modeJustChangedFlag = 0;

void setup() {
   // Set the internal pullup resistor on modePin
   pinMode(modePin, INPUT_PULLUP);
   // add the interrupt to the mode pin
   attachInterrupt(modePin, changeMode, RISING);
   pinMode(irPin,       INPUT);
   pinMode(buzzerPin,   OUTPUT);
   pinMode(modeLeds[0], OUTPUT);
   pinMode(modeLeds[1], OUTPUT);
   pinMode(modeLeds[2], OUTPUT);

   pinMode(offTargetA, OUTPUT);
   pinMode(offTargetB, OUTPUT);
   pinMode(onTargetA,  OUTPUT);
   pinMode(onTargetB,  OUTPUT);

   digitalWrite(modeLeds[currentMode], HIGH);

   Serial.begin(9600);
   Serial.println("3 Weapon Scoring Box");
   Serial.println("====================");

   resetValues();
}

void loop() {
   checkIfModeChanged();
   //irReceive();  // this takes to long to run and slows the main loop down
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
}

//====================
// Mode pin interupt
//====================
void changeMode() {
   modeJustChangedFlag = 1;
}

//============================
// Sets the correct mode led
//============================
void setModeLeds() {
   for (int i = 0; i < 3; i++) {
      digitalWrite(modeLeds[i], LOW);
   }
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
      Serial.print("Mode Changed to: ");
      Serial.println(currentMode);
      //delay(200);
      modeJustChangedFlag = 0;
   }
}

//=====================
// Check if IR sensor
//=====================
void irReceive() {
   //look for a header pulse from the IR Receiver
   long lengthHeader = pulseIn(irPin, LOW);
   if (lengthHeader > 5000 && (millis() - lastTimeIRChecked > 100)) {
      //step through each of the 32 bits that streams from the remote
      int byteValue = 0;
      for (int i = 1; i <= 32; i++) {
         bit = pulseIn(irPin, HIGH);

         //read the 8 bits that are specifically the key code
         //use bitwise operations to convert binary to decimal
         if (i > 16 && i <= 24) {
            if (bit > 1000)
               byteValue = byteValue + (1 << (i - 17));
         }
      }

      //send the key code to the processing.org program
      Serial.println(byteValue);
      Serial.flush();

      if (byteValue == 5) {
         changeMode();
      }
   }
   lastTimeIRChecked = millis();
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
                  Serial.write("A");
               // otherwise we have an offTarget
               } else {
                  digitalWrite(offTargetA, HIGH);
                  digitalWrite(buzzerPin,  HIGH);
                  Serial.write("C");
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
                  Serial.write("B");
               // otherwise we have an offTarget
               } else {
                  digitalWrite(offTargetB, HIGH);
                  digitalWrite(buzzerPin,  HIGH);
                  Serial.write("D");
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
      if (weaponA < 1024 - voltageThresh) {
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
                  Serial.println("A");
               // offTarget
               } else {
                  Serial.write("D");
               }
            }
         }
      } else { // Nothing happening
          millisPastA = millis();
      }
   }

   // weapon B
   if (hitB == false) {
      // ignore if we've hit
      if (weaponB < 1024 - voltageThresh) {
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
                  Serial.println("B");
               // offTarget
               } else {
                  Serial.write("D");
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
                  Serial.write("A");
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
                  Serial.write("B");
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
   Serial.print("R");
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
