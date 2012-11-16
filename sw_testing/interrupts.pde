// constants won't change. They're used here to 
// set pin numbers:
const int buttonPin = 0;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
volatile int state = LOW;

// Install the interrupt routine.
ISR(INT0_vect) {
  // check the value again - since it takes some time to
  // activate the interrupt routine, we get a clear signal.
  value = digitalRead(buttonPin);
}

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);      
  attachInterrupt(buttonPin, stateChange, CHANGE);
}

void loop() {
  digitalWrite(ledPin, state);
}

void stateChange() {
  state = !state;
}
