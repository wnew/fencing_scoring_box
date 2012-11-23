//===========================================================================//
//                                                                           //
//  Desc: Arduino Code to implement a foil scoring apparatus                 //
//  Dev:  DigitalWestie & Wnew                                               //
//  Date: Nov 2012                                                           //
//  Notes: Origonal code from digitalwestie on github                        //
//         Plan to edit to include other weapons                             //
//                                                                           //
//===========================================================================//

int offTargetA = 10;        // Off Target A Light
int offTargetB = 11;        // Off Target B Light
int onTargetA  = 9;        // On Target A Light
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

int lockOut        = 300;    // the lockout time between hits for epee is 300ms
int minHitDuration = 14;     // the minimum amount of time the tip needs to be depressed

boolean hitA = false;
boolean hitB = false;

boolean isFirstHit = true;

int voltageThresh = 340;     // the threshold that the scoring triggers on (1024/3)


void setup() {
   pinMode(offTargetA, OUTPUT);
   pinMode(offTargetB, OUTPUT);
   pinMode(onTargetA,  OUTPUT);
   pinMode(onTargetB,  OUTPUT);
   
   pinMode(weaponPinA, INPUT);     
   pinMode(weaponPinB, INPUT);     
   pinMode(lamePinA,   INPUT);    
   pinMode(lamePinB,   INPUT);
   
   Serial.begin(9600);
   Serial.println("Start");
}

void loop()
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
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + lockOut > millis())))
         {
            if  (millis() <= (millisPastA + minHitDuration)) // if 14ms or more have past we have a hit
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
               }
               else
               {
                  //offTarget
                  digitalWrite(offTargetA, HIGH);
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
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + lockOut > millis())))
         {
            if  (millis() <= (millisPastB + minHitDuration)) // if 14ms or more have past we have a hit
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
                }
                else
                {
                   // offTarget
                   digitalWrite(offTargetB, HIGH);
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
      if (millis() >= (millisPastFirst + lockOut))
      {
         // time for next action is up!
         delay(1500); 
         resetValues();      
      }
   }
}

void resetValues()
{
   // red side wont reset without fiddling with other side!!
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
