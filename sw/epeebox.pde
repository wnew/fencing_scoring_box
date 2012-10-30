//  From digitalwestie on github
//  Plan to edit to include other weapons and tests
//  epeebox.pde


int onTargetA = 12;         // On Target A Light
int onTargetB = 13;         // On Target B Light

int weaponPinA = 0;         // Weapon A pin
int weaponPinB = 1;         // Weapon B pin

int weaponA = 0;
int weaponB = 0;

long millisPastA     = 0;
long millisPastB     = 0;
long millisPastFirst = 0;

int lockOut        = 40;    // the lockout time between hits for epee is 40ms
int minHitDuration = 2;     // the minimum amount of time the tip needs to be depressed

boolean hitA = false;
boolean hitB = false;

boolean isFirstHit = true;

int voltageThresh = 500;         // the threshold that the scoring triggers on


void setup() {
  pinMode(onTargetA,  OUTPUT);
  pinMode(onTargetB,  OUTPUT);
  
  pinMode(weaponPinA, INPUT);     
  pinMode(weaponPinB, INPUT);     
  
  Serial.begin(9600);
  Serial.println("Start");
}

void loop()
{
  weaponA = analogRead(weaponPinA);
  weaponB = analogRead(weaponPinB);
  
  signalHits();  
 
  // weapon A 
  if (hitA == false) //ignore if we've hit
  {
    if (weaponA < voltageThresh)
    {
        if((isFirstHit == true) || ((isFirstHit == false) && (millisPastA + lockOut > millis())))
        {
            if  (millis() <= (millisPastA + minHitDuration)) // if 14ms or more have past we have a hit
            {
                hitA = true;
                if(isFirstHit)
                {
                  millisPastFirst = millis();
                }
                // epee is always onTarget
                digitalWrite(onTargetA, HIGH);
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
    if (weaponB < voltageThresh)
    {
        if((isFirstHit == true) || ((isFirstHit == false) && (millisPastA + lockOut > millis())))
        {
            if  (millis() <= (millisPastB + minHitDuration)) // if 14ms or more have past we have a hit
            {
                if(isFirstHit)
                {
                  millisPastFirst = millis();
                }
                // epee is always onTarget
                digitalWrite(onTargetB, HIGH);
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
   digitalWrite(onTargetA, LOW);
   digitalWrite(onTargetB, LOW);
     
   millisPastA = millis();
   millisPastB = millis();
   millisPastFirst = 0;

   hitA = false;
   hitB = false;

   isFirstHit = true;
   
   delay(500);
}
