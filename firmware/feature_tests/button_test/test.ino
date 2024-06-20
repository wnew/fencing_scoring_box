const uint8_t modePin    =  2;        // Mode change button interrupt pin 0 (digital pin 2)
#define BAUDRATE   57600  // baudrate of the serial debug interface


void setup() {

   Serial.begin(BAUDRATE);
   while (!Serial);
   Serial.println("3 Weapon Scoring Box");
   Serial.println("====================");
   Serial.print  ("Mode : ");
   //Serial.println(digitalPinToInterrupt(modePin));

   // set the internal pullup resistor on modePin
   pinMode(modePin, INPUT_PULLUP);
}

bool last_button_state = LOW;
int count = 0;
//============
// Main Loop
//============
void loop() {
    if (last_button_state != digitalRead(modePin) && last_button_state == HIGH)
    	count++;
    	Serial.println(count);
        last_button_state = digitalRead(modePin);
    	delay(100);
}