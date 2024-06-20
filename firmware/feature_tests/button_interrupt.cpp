//===========================================================================//
//                                                                           //
//  Desc:    Arduino Code to implement a fencing scoring apparatus           //
//  Dev:     Wnew                                                            //
//  Date:    Nov  2012                                                       //
//  Updated: Sept 2015                                                       //
//  Notes:   1. Basis of algorithm from digitalwestie on github. Thanks Mate //
//           2. Used uint8_t instead of int where possible to optimise       //
//           3. Set ADC prescaler to 16 faster ADC reads                     //
//                                                                           //
//  To do:   1. Could use shift reg on lights and mode LEDs to save pins     //
//           2. Implement short circuit LEDs (already provision for it)      //
//           3. Set up debug levels correctly                                //
//                                                                           //
//===========================================================================//

//============
// #defines
//============
//TODO: set up debug levels correctly
#define DEBUG 0
//#define TEST_LIGHTS       // turns on lights for a second on start up
//#define TEST_ADC_SPEED    // used to test sample rate of ADCs
//#define REPORT_TIMING     // prints timings over serial interface
#define BUZZERTIME  1000  // length of time the buzzer is kept on after a hit (ms)
#define LIGHTTIME   3000  // length of time the lights are kept on after a hit (ms)
#define BAUDRATE   57600  // baudrate of the serial debug interface

//============
// Pin Setup
//============
const uint8_t shortLEDA  =  8;    // Short Circuit A Light
const uint8_t onTargetA  =  9;    // On Target A Light
const uint8_t offTargetA = 10;    // Off Target A Light
const uint8_t offTargetB = 11;    // Off Target B Light
const uint8_t onTargetB  = 12;    // On Target B Light
const uint8_t shortLEDB  = 13;    // Short Circuit A Light

const uint8_t groundPinA = A0;    // Ground A pin - Analog
const uint8_t weaponPinA = A1;    // Weapon A pin - Analog
const uint8_t lamePinA   = A2;    // Lame   A pin - Analog (Epee return path)
const uint8_t lamePinB   = A3;    // Lame   B pin - Analog (Epee return path)
const uint8_t weaponPinB = A4;    // Weapon B pin - Analog
const uint8_t groundPinB = A5;    // Ground B pin - Analog

const uint8_t modePin    =  2;        // Mode change button interrupt pin 0 (digital pin 2)
const uint8_t buzzerPin  =  3;        // buzzer pin
const uint8_t modeLeds[] = {4, 5, 6}; // LED pins to indicate weapon mode selected {f e s}

//=========================
// values of analog reads
//=========================
int weaponA = 0;
int weaponB = 0;
int lameA   = 0;
int lameB   = 0;
int groundA = 0;
int groundB = 0;

//=======================
// depress and timeouts
//=======================
long depressAtime = 0;
long depressBtime = 0;
bool lockedOut    = false;

//==========================
// Lockout & Depress Times
//==========================
// the lockout time between hits for foil is 300ms +/-25ms
// the minimum amount of time the tip needs to be depressed for foil 14ms +/-1ms
// the lockout time between hits for epee is 45ms +/-5ms (40ms -> 50ms)
// the minimum amount of time the tip needs to be depressed for epee 2ms
// the lockout time between hits for sabre is 120ms +/-10ms
// the minimum amount of time the tip needs to be depressed (in contact) for sabre 0.1ms -> 1ms
// These values are stored as micro seconds for more accuracy
//                         foil   epee   sabre
const long lockout [] = {300000,  45000, 120000};  // the lockout time between hits
const long depress [] = { 14000,   2000,   1000};  // the minimum amount of time the tip needs to be depressed



//=================
// mode constants
//=================
const uint8_t FOIL_MODE  = 0;
const uint8_t EPEE_MODE  = 1;
const uint8_t SABRE_MODE = 2;

uint8_t currentMode = EPEE_MODE;

bool modeJustChangedFlag = false;

//=========
// states
//=========
boolean depressedA  = false;
boolean depressedB  = false;
boolean hitOnTargA  = false;
boolean hitOffTargA = false;
boolean hitOnTargB  = false;
boolean hitOffTargB = false;

#ifdef TEST_ADC_SPEED
long now;
long loopCount = 0;
bool done = false;
#endif


//================
// Configuration
//================
void setup() {
   // set the internal pullup resistor on modePin
   pinMode(modePin, INPUT_PULLUP);

   // add the interrupt to the mode pin (interrupt 0)
   attachInterrupt(digitalPinToInterrupt(modePin), changeMode, FALLING);
   pinMode(modeLeds[0], OUTPUT);
   pinMode(modeLeds[1], OUTPUT);
   pinMode(modeLeds[2], OUTPUT);

   // set the light pins to outputs
   pinMode(offTargetA, OUTPUT);
   pinMode(offTargetB, OUTPUT);
   pinMode(onTargetA,  OUTPUT);
   pinMode(onTargetB,  OUTPUT);
   pinMode(shortLEDA,  OUTPUT);
   pinMode(shortLEDB,  OUTPUT);
   pinMode(buzzerPin,  OUTPUT);

   digitalWrite(modeLeds[currentMode], HIGH);

#ifdef TEST_LIGHTS
   testLights();
#endif

   // this optimises the ADC to make the sampling rate quicker
   //adcOpt();

   Serial.begin(BAUDRATE);
   Serial.println("3 Weapon Scoring Box");
   Serial.println("====================");
   Serial.print  ("Mode : ");
   Serial.println(currentMode);
}


//============
// Main Loop
//============
void loop() {
   // use a while as a main loop as the loop() has too much overhead for fast analogReads
   // we get a 3-4% speed up on the loop this way
   while(1) {
      checkIfModeChanged();
   }
}

// the following variables are unsigned long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers
bool last_button_state = HIGH;
//=====================
// Mode pin interrupt
//=====================
void changeMode() {
   //if (digitalRead(modePin) == LOW && last_button_state == HIGH) {
      if ((millis() - lastDebounceTime) > debounceDelay && digitalRead(modePin) == LOW) {
         //Serial.println("Interrupt");
         modeJustChangedFlag = true;
      //} else {
         //modeJustChangedFlag = false;
      }
      lastDebounceTime = millis();
   //}
   //last_button_state = digitalRead(modePin);
}


//========================
// Run when mode changed
//========================
void checkIfModeChanged() {
   delay(100);
   if (modeJustChangedFlag && digitalRead(modePin) == LOW) {
      Serial.println("Button Pressed");
      modeJustChangedFlag = false;
   }
}