
void setup() {
  Serial.begin(115200);
  Serial.println("starting capture");
}

void loop() {
  Serial.println(analogRead(6));
  Serial.println(analogRead(8));
  Serial.println(analogRead(9));
  Serial.println(analogRead(A0));
  Serial.println(analogRead(A1));
  Serial.println(analogRead(A2));
  delay(5000);
}