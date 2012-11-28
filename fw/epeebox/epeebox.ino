//===========================================================================//
//                                                                           //
//  Desc: Arduino Code to implement an epee scoring apparatus                //
//  Dev:  DigitalWestie & Wnew                                               //
//  Date: Nov 2012                                                           //
//  Notes: Origonal code from digitalwestie on github                        //
//         Plan to edit to include other weapons                             // 
//                                                                           //
//===========================================================================//

int onTargetA = 9;          // On Target A Light
int onTargetB = 12;         // On Target B Light

int weaponPinA = 0;         // Weapon A pin
int weaponPinB = 1;         // Weapon B pin
int lamePinA   = 2;         // Lame A pin (Epee return path)
int lamePinB   = 3;         // Lame B pin (Epee return path)

int weaponA    = 0;
int weaponB    = 0;
int lameA      = 0;         
int lameB      = 0;

long millisPastA     = 0;
long millisPastB     = 0;
long millisPastFirst = 0;

int lockOut        = 48;    // the lockout time between hits for epee is 48ms
int minHitDuration = 4;     // the minimum amount of time the tip needs to be depressed

boolean hitA = false;
boolean hitB = false;

boolean isFirstHit = true;

int voltageThresh = 340;         // the threshold that the scoring triggers on (1024/3)


void setup() {
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
      if (weaponA < 1024 - voltageThresh)
      {
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + lockOut > millis())))
         {
            if  (millis() <= (millisPastA + minHitDuration)) // if 14ms or more have past we have a hit
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
         if((isFirstHit == true) || ((isFirstHit == false) && (millisPastFirst + lockOut > millis())))
         {
            if  (millis() <= (millisPastB + minHitDuration)) // if 14ms or more have past we have a hit
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
         delay(3500);
         resetValues();      
      }
   }
}

void resetValues()
{
   // red side wont reset without fiddling with other side!!
   Serial.println("R");
   digitalWrite(onTargetA, LOW);
   digitalWrite(onTargetB, LOW);
     
   //millisPastA = millis();
   //millisPastB = millis();
   millisPastA = 0;
   millisPastB = 0;
   millisPastFirst = 0;

   hitA = false;
   hitB = false;

   isFirstHit = true;
   
   delay(100);
}
