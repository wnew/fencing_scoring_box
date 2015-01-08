
int offTargetA = 13;        // choose the pin for the LED
int offTargetB = 10;        // choose the pin for the LED
int onTargetA = 12;         // choose the pin for the LED
int onTargetB = 11;         // choose the pin for the LED

int weaponPinA = 'A0';         // choose the input pin
int weaponPinB = 'A1';         // choose the input pin
int lamePinA = 'A2';           // choose the input pin
int lamePinB = 'A3';           // choose the input pin

int lameA = 0;
int lameB = 0;

int weaponA = 0;
int weaponB = 0;

long millisPastA = 0;
long millisPastB = 0;
long millisPastFirst = 0;

boolean hitA = false;
boolean hitB = false;

boolean isFirstHit = true;


void setup() {
  pinMode(offTargetA, OUTPUT);      // declare LED as output
  pinMode(offTargetB, OUTPUT);      // declare LED as output
  pinMode(onTargetA, OUTPUT);       // declare LED as output
  pinMode(onTargetB, OUTPUT);       // declare LED as output

  Serial.begin(9600);
  Serial.println("Start");
}

void loop()
{
  weaponA = analogRead(weaponPinA);  // read input value
  weaponB = analogRead(weaponPinB);  // read input value

  lameA = analogRead(lamePinA);  // read input value
  lameB = analogRead(lamePinB);  // read input value

  signalHits();

  //WEAPON B
  if (hitB == false) //ignore if we've hit
  {
    if (weaponB < 500)
    {
        if((isFirstHit == true) || ((isFirstHit == false) && (millisPastA+300 > millis())))
        {
            if  (millis() <= (millisPastB + 14)) //if 14ms or more have past we have a hit
            {
                hitB = true;
                if(isFirstHit)
                {
                  millisPastFirst = millis();
                }
                if (lameA > 500)
                {
                  //onTarget
                  digitalWrite(onTargetB, HIGH);
                  Serial.println("onTargB");
                }
                else
                {
                  //offTarget
                  digitalWrite(offTargetB, HIGH);
                  Serial.println("offTargB");
                }
            }
        }
    }
    else //Nothing happening
    {
        millisPastB = millis();
    }
  }


  //WEAPON A
  if (hitA == false) //ignore if we've hit
  {
    if (weaponA < 500)
    {
        if((isFirstHit == true) || ((isFirstHit == false) && (millisPastA+300 > millis())))
        {
            if  (millis() <= (millisPastA + 14)) //if 14ms or more have past we have a hit
            {
                hitA = true;
                if(isFirstHit)
                {
                  millisPastFirst = millis();
                }
                if (lameB > 500)
                {
                  //onTarget
                  digitalWrite(onTargetA, HIGH);
                  Serial.println("onTargA");
                }
                else
                {
                  //offTarget
                  digitalWrite(offTargetA, HIGH);
                  Serial.println("offTargA");
                }
            }
        }
    }
    else //Nothing happening
    {
        millisPastA = millis();
    }
  }
}

void signalHits()
{

  if (hitB || hitA)
  {
    if (millis() >= (millisPastFirst + 300))
    {
      //time for next action is up!
      delay(2000); //wait for a second
      resetValues();
    }
  }

}

void resetValues()
{

  //RED SIDE WONT RESET WITHOUT FIDDLING WITH OTHER SIDE!!
   digitalWrite(offTargetA, LOW);
   digitalWrite(onTargetA, LOW);
   digitalWrite(offTargetB, LOW);
   digitalWrite(onTargetB, LOW);

   millisPastA = millis();
   millisPastB = millis();
   millisPastFirst = 0;

   hitA = false;
   hitB = false;

   isFirstHit = true;

   delay(500);
}
