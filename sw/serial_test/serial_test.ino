void setup() 
{
  Serial.begin(9600);
}

void loop()
{
  Serial.println("hitA");
  delay(2000);
  Serial.println("hitB");
  delay(2000);
  Serial.println("offTargetA");
  delay(2000);
  Serial.println("offTargetB");
  delay(2000);
}
