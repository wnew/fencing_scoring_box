int pin = 7;
int tot_pulse = 0;
unsigned long pulsehigh, pulselow;

void setup()
{
  pinMode(pin, INPUT);
  Serial.begin(9600);
  Serial.println("Start");
}

void loop()
{
  pulsehigh = pulseIn(pin, HIGH);
  //pulselow = pulseIn(pin, LOW);
  if(pulsehigh != 0)
     tot_pulse = tot_pulse + pulsehigh;
  else
     tot_pulse = 0;
  Serial.println(pulsehigh);
  Serial.println("Total");
  Serial.println(tot_pulse);
}
