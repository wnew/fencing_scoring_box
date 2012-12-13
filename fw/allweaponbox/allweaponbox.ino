//===========================================================================//
//                                                                           //
//  Desc: Arduino Code to implement a foil scoring apparatus                 //
//  Dev:  DigitalWestie & Wnew                                               //
//  Date: Nov 2012                                                           //
//  Notes: Origonal code from digitalwestie on github                        //
//         Plan to edit to include other weapons                             //
//                                                                           //
//===========================================================================//

int onTargetA  = 7;         // On Target A Light
int offTargetA = 8;         // Off Target A Light
int shortLEDA  = 9;         // Short Circuit A Light
int shortLEDB  = 10;        // Short Circuit A Light
int offTargetB = 11;        // Off Target B Light
int onTargetB  = 12;        // On Target B Light

int weaponPinA = 0;         // Weapon A pin
int weaponPinB = 1;         // Weapon B pin
int lamePinA   = 2;         // Lame A pin
int lamePinB   = 3;         // Lame B pin

int weaponA    = 0;
int weaponB    = 0;
int lameA      = 0;
int lameB      = 0;

long millisPastA     = 0;
long millisPastB     = 0;
long millisPastFirst = 0;

int foilLockout  = 300;    // the lockout time between hits for foil is 300ms +/-25ms
int foilDepress  = 14;     // the minimum amount of time the tip needs to be depressed for foil 14ms +/-1ms
int epeeLockout  = 45;     // the lockout time between hits for epee is 45ms +/-5ms (40ms -> 50ms)
int epeeDepress  = 2;      // the minimum amount of time the tip needs to be depressed for epee
int sabreLockout = 120;    // the lockout time between hits for sabre is 120ms +/-10ms
int sabreDepress = 1;      // the minimum amount of time the tip needs to be depressed for sabre 0.1ms -> 1ms

boolean hitA = false;
boolean hitB = false;

boolean isFirstHit = true;

int voltageThresh = 340;     // the threshold that the scoring triggers on (1024/3)

const int modePin = 0;
int mode = 0;

// mode constants
const int FOIL_MODE  = 0;
const int EPEE_MODE  = 1;
const int SABRE_MODE = 2;

void setup() {

   // add the interrupt to the mode pin
   attachInterrupt(modePin, changeMode, RISING);
   pinMode(modePin, INPUT);

   pinMode(offTargetA, OUTPUT);
   pinMode(offTargetB, OUTPUT);
   pinMode(onTargetA,  OUTPUT);
   pinMode(onTargetB,  OUTPUT);
   
   pinMode(weaponPinA, INPUT);     
   pinMode(weaponPinB, INPUT);     
   pinMode(lamePinA,   INPUT);    
   pinMode(lamePinB,   INPUT);
   
   resetValues();
   
   Serial.begin(9600);
   Serial.println("3 Weapon Scoring Box");
   Serial.println("====================");
}

void loop() {
   if (mode == FOIL_MODE)
      foil();
   if (mode == EPEE_MODE)
      epee();
   if (mode == SABRE_MODE)
      sabre();
}

// when the mode button is pressed this method is run
void changeMode() {
   if (mode == 2)
      mode = 0;
   else
      mode++;
}

void foil()
{
   weaponA = analogRead(weaponPinA);
   weaponB = analogRead(weaponPinB);
   lameA   = analogRead(lamePinA);
   lameB   = analogRead(lamePinB);
   
   signalHits();  
  
   // weapon A 
   if (hitA == false) //ignore if we've hit
   {
      if (weaponA > voltageThresh)
      {
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + foilLockout > millis())))
         {
            if  (millis() <= (millisPastA + foilDepress)) // if 14ms or more have past we have a hit
            {
               hitA = true;
               if(isFirstHit)
               {
                  millisPastFirst = millis();
               }
               if (lameB > voltageThresh)
               {
                  //onTarget
                  digitalWrite(onTargetA, HIGH);
                  Serial.write("A");
               }
               else
               {
                  //offTarget
                  digitalWrite(offTargetA, HIGH);
                  Serial.write("C");
               }
            }
         } 
      }
      else // Nothing happening
      {
         millisPastA = millis();
      }
   }
   
   // weapon B
   if (hitB == false) // ignore if we've hit
   {
      if (weaponB > voltageThresh)
      {
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + foilLockout > millis())))
         {
            if  (millis() <= (millisPastB + foilDepress)) // if 14ms or more have past we have a hit
            {
               hitB = true;
               if(isFirstHit)
               {
                  millisPastFirst = millis();
               }
               if (lameA > voltageThresh)
               {
                  // onTarget
                  digitalWrite(onTargetB, HIGH);
                  Serial.write("B");
               }
               else
               {
                  // offTarget
                  digitalWrite(offTargetB, HIGH);
                  Serial.write("D");
               }
            }
         }
      }
      else // nothing happening
      {
         millisPastB = millis();
      }
   }
}

void epee()
{
   weaponA = analogRead(weaponPinA);
   weaponB = analogRead(weaponPinB);
   lameA   = analogRead(lamePinA);
   lameB   = analogRead(lamePinB);
  
   signalHits();  
 
   // weapon A 
   if (hitA == false) //ignore if we've hit
   {
      if (weaponA < 1024 - voltageThresh)
      {
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + epeeLockout > millis())))
         {
            if  (millis() <= (millisPastA + epeeDepress)) // if 14ms or more have past we have a hit
            {
               if (lameA > voltageThresh)
               {
                  // onTarget
                  if(isFirstHit)
                  {
                     millisPastFirst = millisPastA;
                     isFirstHit = false;
                  }
                  digitalWrite(onTargetA, HIGH);
                  hitA = true;
                  Serial.println("A");
               }
               else
               {
                  // offTarget
                  Serial.write("D");
               }
            }
         } 
      }
      else // Nothing happening
      {
          millisPastA = millis();
      }
   }
   
   // weapon B
   if (hitB == false) // ignore if we've hit
   {
      if (weaponB < 1024 - voltageThresh)
      {
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + epeeLockout > millis())))
         {
            if  (millis() <= (millisPastB + epeeDepress)) // if 14ms or more have past we have a hit
            {
               if (lameB > voltageThresh)
               {
                  // onTarget
                  if(isFirstHit)
                  {
                     millisPastFirst = millisPastB;
                     isFirstHit = false;
                  }
                  digitalWrite(onTargetB, HIGH);
                  hitB = true;
                  Serial.println("B");
               }
               else
               {
                  // offTarget
                  Serial.write("D");
               }
            }
         }
      }
      else // nothing happening
      {
         millisPastB = millis();
      }
   }
}

void sabre()
{
   weaponA = analogRead(weaponPinA);
   weaponB = analogRead(weaponPinB);
   lameA   = analogRead(lamePinA);
   lameB   = analogRead(lamePinB);
   
   signalHits();  
  
   // weapon A 
   if (hitA == false) //ignore if we've hit
   {
      if (weaponA > voltageThresh)
      {
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + sabreLockout > millis())))
         {
            if  (millis() <= (millisPastA + sabreDepress)) // if 14ms or more have past we have a hit
            {
               hitA = true;
               if(isFirstHit)
               {
                  millisPastFirst = millis();
               }
               if (lameB > voltageThresh)
               {
                  //onTarget
                  digitalWrite(onTargetA, HIGH);
                  Serial.write("A");
               }
            }
         } 
      }
      else // Nothing happening
      {
         millisPastA = millis();
      }
   }
   
   // weapon B
   if (hitB == false) // ignore if we've hit
   {
      if (weaponB > voltageThresh)
      {
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + sabreLockout > millis())))
         {
            if  (millis() <= (millisPastB + sabreDepress)) // if 14ms or more have past we have a hit
            {
               hitB = true;
               if(isFirstHit)
               {
                  millisPastFirst = millis();
               }
               if (lameA > voltageThresh)
               {
                  // onTarget
                  digitalWrite(onTargetB, HIGH);
                  Serial.write("B");
               }
            }
         }
      }
      else // nothing happening
      {
         millisPastB = millis();
      }
   }
}

void signalHits()
{
   if (hitA || hitB)
   {
      if (mode == FOIL_MODE)
         if (millis() >= (millisPastFirst + foilLockout))
         {
            // time for next action is up!
            delay(1500); 
            resetValues();      
         }
      if (mode == EPEE_MODE)
         if (millis() >= (millisPastFirst + epeeLockout))
         {
            // time for next action is up!
            delay(1500); 
            resetValues();      
         }
      if (mode == SABRE_MODE)
         if (millis() >= (millisPastFirst + sabreLockout))
         {
            // time for next action is up!
            delay(1500); 
            resetValues();      
         }
   }
}

void resetValues()
{
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
