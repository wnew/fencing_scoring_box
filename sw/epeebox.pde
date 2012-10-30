//  From digitalwestie on github
//  Plan to edit to include other weapons and tests
//  epeebox.pde


int onTargetA  = 12;        // On Target A Light
int onTargetB  = 13;        // On Target B Light

int weaponPinA = 0;         // Weapon A pin
int weaponPinB = 1;         // Weapon B pin

int weaponA = 0;
int weaponB = 0;

long millisPastA = 0;
long millisPastB = 0;
long millisPastFirst = 0;

boolean hitA = false;
boolean hitB = false;

boolean isFirstHit = true;


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
    if (weaponA < 500)
    {
        if((isFirstHit == true) || ((isFirstHit == false) && (millisPastA+300 > millis())))
        {
            if  (millis() <= (millisPastA + 14)) // if 14ms or more have past we have a hit
            {
                hitA = true;
                if(isFirstHit)
                {
                  millisPastFirst = millis();
                }
                //onTarget
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
    if (weaponB < 500)
    {
        if((isFirstHit == true) || ((isFirstHit == false) && (millisPastA+300 > millis())))
        {
            if  (millis() <= (millisPastB + 14)) // if 14ms or more have past we have a hit
            {
                if(isFirstHit)
                {
                  millisPastFirst = millis();
                }
                // onTarget
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
    if (millis() >= (millisPastFirst + 300))
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
