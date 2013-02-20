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
const int onTargetA  = 7;         // On Target A Light
const int offTargetA = 8;         // Off Target A Light
const int shortLEDA  = 9;         // Short Circuit A Light
const int shortLEDB  = 10;        // Short Circuit A Light
const int offTargetB = 11;        // Off Target B Light
const int onTargetB  = 12;        // On Target B Light

const int weaponPinA = 0;         // Weapon A pin
const int weaponPinB = 1;         // Weapon B pin
const int lamePinA   = 2;         // Lame A pin
const int lamePinB   = 3;         // Lame B pin
const int gndPinA    = 4;         // Lame A pin
const int gndPinB    = 5;         // Lame B pin

const int irPin      = 6;         // IR receiver pin
const int modePin    = 0;         // Mode change button pin

int mode = 0;

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
const int foilLockout  = 300;    // the lockout time between hits for foil is 300ms +/-25ms
const int foilDepress  = 14;     // the minimum amount of time the tip needs to be depressed for foil 14ms +/-1ms
const int epeeLockout  = 45;     // the lockout time between hits for epee is 45ms +/-5ms (40ms -> 50ms)
const int epeeDepress  = 2;      // the minimum amount of time the tip needs to be depressed for epee
const int sabreLockout = 120;    // the lockout time between hits for sabre is 120ms +/-10ms
const int sabreDepress = 1;      // the minimum amount of time the tip needs to be depressed for sabre 0.1ms -> 1ms

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

   // add the interrupt to the mode pin
   attachInterrupt(modePin, changeMode, RISING);
   pinMode(irPin, INPUT);

   pinMode(offTargetA, OUTPUT);
   pinMode(offTargetB, OUTPUT);
   pinMode(onTargetA,  OUTPUT);
   pinMode(onTargetB,  OUTPUT);

   pinMode(weaponPinA, INPUT);
   pinMode(weaponPinB, INPUT);
   pinMode(lamePinA,   INPUT);
   pinMode(lamePinB,   INPUT);
   pinMode(gndPinA,    INPUT);
   pinMode(gndPinB,    INPUT);

   resetValues();

   Serial.begin(9600);
   Serial.println("3 Weapon Scoring Box");
   Serial.println("====================");
}

void loop() {
   checkIfModeChanged();
   irReceive();
   if (mode == FOIL_MODE)
      foil();
   if (mode == EPEE_MODE)
      epee();
   if (mode == SABRE_MODE)
      sabre();
}

//====================
// Mode pin interupt
//====================
void changeMode() {
   modeJustChangedFlag = 1;
}

//========================
// Run when mode changed
//========================
void checkIfModeChanged() {
 if (modeJustChangedFlag) {
      if (digitalRead(modePin)) {
         if (mode == 2)
            mode = 0;
         else
            mode++;
      }
      Serial.print("Mode Changed to: ");
      Serial.println(mode);
      delay(200);
      modeJustChangedFlag = 0;
   }
}

//=====================
// Check if IR sensor
//=====================
void irReceive() {
   //look for a header pulse from the IR Receiver
   long lengthHeader = pulseIn(irPin, LOW);
   if(lengthHeader > 5000 && (millis() - lastTimeIRChecked > 100)) {
      //step through each of the 32 bits that streams from the remote
      int byteValue = 0;
      for(int i = 1; i <= 32; i++) {
         bit = pulseIn(irPin, HIGH);

         //read the 8 bits that are specifically the key code
         //use bitwise operations to convert binary to decimal
         if (i > 16 && i <= 24)
            if(bit > 1000)
               byteValue = byteValue + (1 << (i - 17));
       }

      //send the key code to the processing.org program
      Serial.println(byteValue);
      Serial.flush();

      if (byteValue == 5)
         changeMode();
   }
   lastTimeIRChecked = millis();
}

//===================
// Main foil method
//===================
void foil() {
   weaponA = analogRead(weaponPinA);
   weaponB = analogRead(weaponPinB);
   lameA   = analogRead(lamePinA);
   lameB   = analogRead(lamePinB);

   signalHits();

   // weapon A
   if (hitA == false) {
      // ignore if we've hit
      if (weaponA > voltageThresh) {
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + foilLockout > millis()))) {
            // if foil depress time has past we have a hit
            if  (millis() <= (millisPastA + foilDepress)) {
               hitA = true;
               if(isFirstHit) {
                  millisPastFirst = millis();
               }
               // if other lame hit we have an onTarget
               if (lameB > voltageThresh) {

                  digitalWrite(onTargetA, HIGH);
                  Serial.write("A");
               // otherwise we have an offTarget
               } else {
                  digitalWrite(offTargetA, HIGH);
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
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + foilLockout > millis()))) {
            // if foil depress time has past we have a hit
            if  (millis() <= (millisPastB + foilDepress)) {
               hitB = true;
               if(isFirstHit) {
                  millisPastFirst = millis();
               }
               // if other lame hit we have an onTarget
               if (lameA > voltageThresh) {
                  digitalWrite(onTargetB, HIGH);
                  Serial.write("B");
               // otherwise we have an offTarget
               } else {
                  digitalWrite(offTargetB, HIGH);
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
   weaponA = analogRead(weaponPinA);
   weaponB = analogRead(weaponPinB);
   lameA   = analogRead(lamePinA);
   lameB   = analogRead(lamePinB);

   signalHits();

   // weapon A
   if (hitA == false) {
      // ignore if we've hit
      if (weaponA < 1024 - voltageThresh) {
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + epeeLockout > millis()))) {
            // if epee depress time has past we have a hit
            if  (millis() <= (millisPastA + epeeDepress)) {
               // onTarget
               if (lameA > voltageThresh) {
                  if(isFirstHit) {
                     millisPastFirst = millisPastA;
                     isFirstHit = false;
                  }
                  digitalWrite(onTargetA, HIGH);
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
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + epeeLockout > millis()))) {
            // if epee depress time has past we have a hit
            if  (millis() <= (millisPastB + epeeDepress)) {
               // onTarget
               if (lameB > voltageThresh) {
                  if(isFirstHit) {
                     millisPastFirst = millisPastB;
                     isFirstHit = false;
                  }
                  digitalWrite(onTargetB, HIGH);
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
   weaponA = analogRead(weaponPinA);
   weaponB = analogRead(weaponPinB);
   lameA   = analogRead(lamePinA);
   lameB   = analogRead(lamePinB);

   signalHits();

   // weapon A
   if (hitA == false) {
      //ignore if we've hit
      if (weaponA > voltageThresh) {
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + sabreLockout > millis()))) {
            // if sabre depress time has past we have a hit
            if  (millis() <= (millisPastA + sabreDepress)) {
               hitA = true;
               if(isFirstHit) {
                  millisPastFirst = millis();
               }
               //onTarget
               if (lameB > voltageThresh) {
                  digitalWrite(onTargetA, HIGH);
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
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + sabreLockout > millis()))) {
            // if sabre depress time has past we have a hit
            if  (millis() <= (millisPastB + sabreDepress)) {
               hitB = true;
               if(isFirstHit) {
                  millisPastFirst = millis();
               }
               // onTarget
               if (lameA > voltageThresh) {
                  digitalWrite(onTargetB, HIGH);
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
      if (mode == FOIL_MODE)
         // if lockout time is up
         if (millis() >= (millisPastFirst + foilLockout)) {
            delay(1500);
            resetValues();
         }
      if (mode == EPEE_MODE)
         // if lockout time is up
         if (millis() >= (millisPastFirst + epeeLockout)) {
            delay(1500);
            resetValues();
         }
      if (mode == SABRE_MODE)
         // if lockout time is up
         if (millis() >= (millisPastFirst + sabreLockout)) {
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
   digitalWrite(offTargetA, LOW);
   digitalWrite(onTargetA,  LOW);
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
