//===========================================================================//
//                                                                           //
//  Desc:    Arduino Code to implement a fencing scoring apparatus           //
//  Dev:     Wnew                                                            //
//  Date:    Nov  2012                                                       //
//  Updated: Dec  2017                                                       //
//  Notes:   1. Used uint8_t instead of int where possible to optimise       //
//                                                                           //
//  To do:   1. Implement short circuit LEDs (already provision for it)      //
//           2. Test Sabre code                                              //
//           3. Implement whip-over for sabre                                //
//                                                                           //
//===========================================================================//

// Fencers are referred to as Green and Red. Green on the left and red on the right.
// The pins of the connectors are referred to as A-B--C
// Where A is: the return path for Epee, the Lame for Foil and Sabre
// Where B is: live for all three weapons, it connects to the blade (there is debate whether A or B is used in Sabre) 
// Where C is: ground for both Epee, Foil and Sabre. Connected to the guard of all 3 weapons.
// The order of the weapons is in alphabetical order, epee, foil sabre.

//============
// #includes
//============
#include <Adafruit_NeoPixel.h>  // adafruit's library for neopixel displays

//============
// #defines
//============
#define DEBUG
//#define TEST_LIGHTS       // turns on lights for a second on start up
//#define TEST_ADC_SPEED    // used to test sample rate of ADCs
//#define REPORT_TIMING     // prints timings over serial interface
#define NEOPIXELS         // if this is set then sketch uses the neopixel display, if not then individual leds per pin are assumed.
#define BUZZERTIME  1000  // length of time the buzzer is kept on after a hit (ms)
#define LIGHTTIME   3000  // length of time the lights are kept on after a hit (ms)
#define BAUDRATE   57600  // baudrate of the serial debug interface

#define LED_PIN       15  // neopixels data pin
#define NUMPIXELS     40  // number of NeoPixels on display


// initialise the neopixel class
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

//============
// Pin Setup
//============
const uint8_t shortLEDRed  = 10;    // Short Circuit Red Light
const uint8_t offTargetRed = 16;    // Off Target Red Light
const uint8_t onTargetRed  = 14;    // On Target Red Light
const uint8_t onTargetGrn  = 3;    // On Target Grn Light
const uint8_t offTargetGrn = 4;    // Off Target Grn Light
const uint8_t shortLEDGrn  = 5;    // Short Circuit Grn Light

// PCB rev1
//const uint8_t groundPinGrn = A0;    // Ground A pin - Analog
//const uint8_t weaponPinGrn = A2;    // Weapon A pin - Analog
//const uint8_t lamePinGrn   = A3;    // Lame   A pin - Analog (Epee return path)
//const uint8_t lamePinRed   = A0;    // Lame   B pin - Analog (Epee return path)
//const uint8_t weaponPinRed = A1;    // Weapon B pin - Analog
//const uint8_t groundPinRed = A5;    // Ground B pin - Analog

// PCB rev2 and rev3
const uint8_t groundPinGrn = 6;    // Ground A pin - Analog
const uint8_t weaponPinGrn = 8;    // Weapon A pin - Analog
const uint8_t lamePinGrn   = 9;    // Lame   A pin - Analog (Epee return path)
const uint8_t lamePinRed   = A0;    // Lame   B pin - Analog (Epee return path)
const uint8_t weaponPinRed = A1;    // Weapon B pin - Analog
const uint8_t groundPinRed = A2;    // Ground B pin - Analog


const uint8_t modePin    =  2;        // Mode change button interrupt pin 0 (digital pin 2)
const uint8_t buzzerPin  =  A3;       // buzzer pin

// there are not enough pins on the pro micro to have 3 pins dedicated to the mode LEDs
// if you are using another arduino with enough pins, you can set this to pins that 
// arent used for other things.
// the mode is currently signaled by setting:
// red on target LED for Foil
// grn on target LED for Epee
// both Red and Grn on target LEDs for Sabre
//const uint8_t modeLeds[] = {7, 8, 9}; // LED pins to indicate weapon mode selected {e f s}

//=================================
// initial values of analog reads
//=================================
int grnA = 0;
int redA = 0;
int grnB = 0;
int redB = 0;
int grnC = 0;
int redC = 0;

//=======================
// depress and timeouts
//=======================
long depressGrnTime = 0;
long depressRedTime = 0;
bool lockedOut      = false;

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
const uint8_t EPEE_MODE  = 0;
const uint8_t FOIL_MODE  = 1;
const uint8_t SABRE_MODE = 2;

// set the initial mode to Epee
uint8_t currentMode = EPEE_MODE;

bool modeJustChangedFlag = false;

//=========
// states
//=========
boolean depressedGrn  = false;
boolean depressedRed  = false;
boolean hitOnTargGrn  = false;
boolean hitOffTargGrn = false;
boolean hitOnTargRed  = false;
boolean hitOffTargRed = false;

#ifdef TEST_ADC_SPEED
long now;
long loopCount = 0;
bool done = false;
#endif


//================
// Configuration
//================
void setup() {
#ifdef DEBUG
   Serial.begin(BAUDRATE);  // start serial connection
   delay(200);              // wait a short time to ensure the serial is up before writing to it
   Serial.println("3 Weapon Scoring Box");
   Serial.println("====================");
   Serial.print  ("Mode : ");
   Serial.println(currentMode);
#endif

   // set the internal pullup resistor on modePin
   pinMode(modePin, INPUT_PULLUP);

   // add the interrupt to the mode pin (pin 2 is interrupt0 on the Uno and interrupt1 on the Micro)
   // change to modePin-2 for the Uno
   attachInterrupt(digitalPinToInterrupt(modePin), changeMode, FALLING);

   // set the mode leds to outputs and turn the correct one on
   //pinMode(modeLeds[0], OUTPUT);
   //pinMode(modeLeds[1], OUTPUT);
   //pinMode(modeLeds[2], OUTPUT);
   //digitalWrite(modeLeds[currentMode], HIGH);

   // set the light pins to outputs
   pinMode(offTargetGrn, OUTPUT);
   pinMode(offTargetRed, OUTPUT);
   pinMode(onTargetGrn,  OUTPUT);
   pinMode(onTargetRed,  OUTPUT);
   //pinMode(shortLEDGrn,  OUTPUT);
   //pinMode(shortLEDRed,  OUTPUT);
   pinMode(buzzerPin,  OUTPUT);


   // initialise the LED display
   pixels.begin();

#ifdef TEST_LIGHTS
   testLights();
#endif

   // this optimises the ADC to make the sampling rate quicker
   // it is not advisable to use this unless you know what you are doing
   // it can become unstable and give strange ADC readings. Be warned.
   //adcOpt();

   setModeLeds();
}


//=============
// ADC config
//=============
void adcOpt() {

   // the ADC only needs a couple of bits, the atmega is an 8 bit micro
   // so sampling only 8 bits makes the values easy/quicker to process
   // unfortunately this method only works on the Due.
   //analogReadResolution(8);

   // Data Input Disable Register
   // disconnects the digital inputs from which ever ADC channels you are using
   // an analog input will be float and cause the digital input to constantly
   // toggle high and low, this creates noise near the ADC, and uses extra 
   // power. Secondly, the digital input and associated DIDR switch have a
   // capacitance associated with them which will slow down your input signal
   // if you’re sampling a highly resistive load 
   DIDR0 = 0x7F;

   // set the prescaler for the ADCs to 16 this allowes the fastest sampling
   bitClear(ADCSRA, ADPS0);
   bitClear(ADCSRA, ADPS1);
   bitSet  (ADCSRA, ADPS2);
}


//============
// Main Loop
//============
void loop() {
   // use a while as a main loop as the loop() has too much overhead for fast analogReads
   // we get a 3-4% speed up on the loop this way
   while(1) {
      checkIfModeChanged();
      // read analog pins
      grnA = analogRead(weaponPinGrn);
      redA = analogRead(weaponPinRed);
      grnB = analogRead(lamePinGrn);
      redB = analogRead(lamePinRed);
      signalHits();
      // decide which weapon is currently selected
      if      (currentMode == EPEE_MODE)
         epee();
      else if (currentMode == FOIL_MODE)
         foil();
      else if (currentMode == SABRE_MODE)
         sabre();

#ifdef TEST_ADC_SPEED
      if (loopCount == 0) {
         now = micros();
      }
      loopCount++;
      if ((micros()-now >= 1000000) && done == false) {
         Serial.print(loopCount);
         Serial.println(" readings in 1 sec");
         done = true;
      }
#endif
   }
}


// the following variables are unsigned long's because the time, measured in milliseconds,
// will quickly overflow an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;   // the debounce time; increase if the receiving multiple interrupts per button press

//==============================
// Mode pin interrupt function
//==============================
void changeMode() {
   if ((millis() - lastDebounceTime) > debounceDelay && digitalRead(modePin) == LOW) {
      modeJustChangedFlag = true;
   }
   lastDebounceTime = millis();
}


//========================
// Run when mode changed
//========================
void checkIfModeChanged() {
   //delay(100);
   if (modeJustChangedFlag && digitalRead(modePin) == LOW) {
      modeJustChangedFlag = false;
      if (currentMode == 2)
         currentMode = 0;
      else
         currentMode++;

      setModeLeds();
#ifdef DEBUG
      Serial.print("Mode changed to: ");
      Serial.println(currentMode);
#endif
   } else {
      modeJustChangedFlag = false;
   }
}


//============================
// Sets the correct mode led
//============================
void setModeLeds() {
   if (currentMode == EPEE_MODE) {
      ledEpee();
      digitalWrite(onTargetGrn, HIGH);
   } else {
      if (currentMode == FOIL_MODE) {
         ledFoil();
         digitalWrite(onTargetRed, HIGH);
      } else {
         if (currentMode == SABRE_MODE){
            ledSabre();
            digitalWrite(onTargetGrn, HIGH);
            digitalWrite(onTargetRed, HIGH);
         }
      }
   }
   long now = millis();
   while (millis() < now + 1000) { // delay to keep the leds on for 1 second
      if (modeJustChangedFlag == true) {
         break;
      }
   }
   //delay(500);
   digitalWrite(onTargetGrn, LOW);
   digitalWrite(onTargetRed, LOW);
   clearLEDs();
}


//===================
// Main foil method
//===================
void foil() {

   long now = micros();
   if (((hitOnTargGrn || hitOffTargGrn) && (depressGrnTime + lockout[0] < now)) || 
       ((hitOnTargRed || hitOffTargRed) && (depressRedTime + lockout[0] < now))) {
      lockedOut = true;
   }

   // weapon Grn
   if (hitOnTargGrn == false && hitOffTargGrn == false) { // ignore if Grn has already hit
      // off target
      if (900 < grnA && redB < 100) {
         if (!depressedGrn) {
            depressGrnTime = micros();
            depressedGrn   = true;
         } else {
            if (depressGrnTime + depress[0] <= micros()) {
               hitOffTargGrn = true;
            }
         }
      } else {
         // on target
         if (400 < grnA && grnA < 600 && 400 < redB && redB < 600) {
            if (!depressedGrn) {
               depressGrnTime = micros();
               depressedGrn   = true;
            } else {
               if (depressGrnTime + depress[0] <= micros()) {
                  hitOnTargGrn = true;
               }
            }
         } else {
            // reset these values if the depress time is short.
            depressGrnTime = 0;
            depressedGrn   = 0;
         }
      }
   }

   // weapon Red
   if (hitOnTargRed == false && hitOffTargRed == false) { // ignore if Red has already hit
      // off target
      if (900 < redA && grnB < 100) {
         if (!depressedRed) {
            depressRedTime = micros();
            depressedRed   = true;
         } else {
            if (depressRedTime + depress[0] <= micros()) {
               hitOffTargRed = true;
            }
         }
      } else {
         // on target
         if (400 < redA && redA < 600 && 400 < grnB && grnB < 600) {
            if (!depressedRed) {
               depressRedTime = micros();
               depressedRed   = true;
            } else {
               if (depressRedTime + depress[0] <= micros()) {
                  hitOnTargRed = true;
               }
            }
         } else {
            // reset these values if the depress time is short.
            depressRedTime = 0;
            depressedRed   = 0;
         }
      }
   }
}


//===================
// Main epee method
//===================
void epee() {
   long now = micros();
   if ((hitOnTargGrn && (depressGrnTime + lockout[1] < now)) ||
       (hitOnTargRed && (depressRedTime + lockout[1] < now))) {
      lockedOut = true;
   }

   // weapon Grn
   //  no hit for Grn yet    && weapon depress    && opponent lame touched
   if (hitOnTargGrn == false) {
      if (400 < grnA && grnA < 600 && 400 < grnB && grnB < 600) {
         if (!depressedGrn) {
            depressGrnTime = micros();
            depressedGrn   = true;
         } else {
            if (depressGrnTime + depress[1] <= micros()) {
               hitOnTargGrn = true;
            }
         }
      } else {
         // reset these values if the depress time is short.
         if (depressedGrn == true) {
            depressGrnTime = 0;
            depressedGrn   = 0;
         }
      }
   }

   // weapon Red
   //  no hit for Red yet    && weapon depress    && opponent lame touched
   if (hitOnTargRed == false) {
      if (400 < redA && redA < 600 && 400 < redB && redB < 600) {
         if (!depressedRed) {
            depressRedTime = micros();
            depressedRed   = true;
         } else {
            if (depressRedTime + depress[1] <= micros()) {
               hitOnTargRed = true;
            }
         }
      } else {
         // reset these values if the depress time is short.
         if (depressedRed == true) {
            depressRedTime = 0;
            depressedRed   = 0;
         }
      }
   }
}


//===================
// Main sabre method
//===================
void sabre() {

   long now = micros();
   if (((hitOnTargGrn || hitOffTargGrn) && (depressGrnTime + lockout[2] < now)) || 
       ((hitOnTargRed || hitOffTargRed) && (depressRedTime + lockout[2] < now))) {
      lockedOut = true;
   }

   // weapon Grn
   if (hitOnTargGrn == false && hitOffTargGrn == false) { // ignore if Grn has already hit
      // on target
      if (400 < grnA && grnA < 600 && 400 < redB && redB < 600) {
         if (!depressedGrn) {
            depressGrnTime = micros();
            depressedGrn   = true;
         } else {
            if (depressGrnTime + depress[2] <= micros()) {
               hitOnTargGrn = true;
            }
         }
      } else {
         // reset these values if the depress time is short.
         depressGrnTime = 0;
         depressedGrn   = 0;
      }
   }

   // weapon Red
   if (hitOnTargRed == false && hitOffTargRed == false) { // ignore if Red has already hit
      // on target
      if (400 < redA && redA < 600 && 400 < grnB && grnB < 600) {
         if (!depressedRed) {
            depressRedTime = micros();
            depressedRed   = true;
         } else {
            if (depressRedTime + depress[2] <= micros()) {
               hitOnTargRed = true;
            }
         }
      } else {
         // reset these values if the depress time is short.
         depressRedTime = 0;
         depressedRed   = 0;
      }
   }
}


//==============
// Signal Hits
//==============
void signalHits() {
   // non time critical, this is run after a hit has been detected
   //if (lockedOut) {
      if (hitOnTargGrn) {
         ledOnTargGrn();
      }
      if (hitOffTargGrn) {
         ledOffTargGrn();
      }
      if (hitOffTargRed) {
         ledOffTargRed();
      }
      if (hitOnTargRed) {
         ledOnTargRed();
      }
      if (hitOnTargGrn || hitOnTargRed || hitOffTargGrn || hitOffTargRed) {
         tone(buzzerPin, 4000, BUZZERTIME);
      }
      if (lockedOut) {
         resetValues();

#ifdef DEBUG
         String serData = String("grnA : ") + grnA + "\n"
                               + "grnB : "  + grnB + "\n"
                               + "redB : "  + redB + "\n"
                               + "redA : "  + redA + "\n";
         Serial.println(serData);
         serData = String("hitOnTargGrn  : ") + hitOnTargGrn  + "\n"
                               + "hitOffTargGrn : "  + hitOffTargGrn + "\n"
                               + "hitOffTargRed : "  + hitOffTargRed + "\n"
                               + "hitOnTargRed  : "  + hitOnTargRed  + "\n"
                               + "Locked Out    : "  + lockedOut   + "\n";
         Serial.println(serData);
#endif
      }
      //resetValues();
   //}
}

//=======================
// Clear the LED matrix
//=======================
void clearLEDs() {
   digitalWrite(shortLEDGrn,  LOW);
   digitalWrite(onTargetGrn,  LOW);
   digitalWrite(offTargetGrn, LOW);
   digitalWrite(offTargetRed, LOW);
   digitalWrite(onTargetRed,  LOW);
   digitalWrite(shortLEDRed,  LOW);

   for(int i=0;i<8;i++) {
      for(int j=0;j<9;j++) {
         pixels.setPixelColor(j*8+i, pixels.Color(0,0,0));
      }
   }
   pixels.show();
}


//======================
// Reset all variables
//======================
// this gets runs once a hit has been triggered, it waits the apropriate
// amount of time before turning off the LEDs and buzzer and resetting 
// all the other required flags
void resetValues() {
   long now = millis();
   // wait before turning off the buzzer
   while (millis() < now + BUZZERTIME) {
      if (modeJustChangedFlag == true) {
         break;
      }
   }
   digitalWrite(buzzerPin,  LOW);
   // wait before turning off the leds
   while (millis() < now + LIGHTTIME-BUZZERTIME) {
      if (modeJustChangedFlag == true) {
         break;
      }
   }
   //delay(LIGHTTIME-BUZZERTIME);
   clearLEDs();

   depressGrnTime = 0;
   depressRedTime = 0;
   depressedGrn   = false;
   depressedRed   = false;
   lockedOut      = false;
   hitOnTargGrn   = false;
   hitOffTargGrn  = false;
   hitOnTargRed   = false;
   hitOffTargRed  = false;
}


//==============
// Test lights
//==============
// This function can be run on start up to show that all the lights are working
void testLights() {
   // turn on grn on target
   ledOnTargGrn();
   delay(500);
   // turn on red on target
   ledOnTargRed();
   delay(500);
   // turn off grn and red on target
   clearLEDs();
   // turn on grn off target
   ledOffTargGrn();
   delay(500);
   // turn on red off target
   ledOffTargRed();
   delay(500);
   // turn off grn and red off target
   clearLEDs();
   // turn on grn short circuit
   ledShortGrn();
   delay(500);
   // turn on red short circuit
   ledShortRed();
   delay(500);
   // turn on grn and red short circuit
   clearLEDs();
   resetValues();
}


//===============================
// Print an E to the LED matrix
//===============================
// This prints an E to the centre of the LED matrix to show that the 
// mode has been changed to Epee.
void ledEpee() {
   clearLEDs();
   pixels.setPixelColor( 2, pixels.Color(23,33,178));
   pixels.setPixelColor( 3, pixels.Color(23,33,178));
   pixels.setPixelColor( 4, pixels.Color(23,33,178));
   pixels.setPixelColor(10, pixels.Color(23,33,178));
   pixels.setPixelColor(18, pixels.Color(23,33,178));
   pixels.setPixelColor(19, pixels.Color(23,33,178));
   pixels.setPixelColor(26, pixels.Color(23,33,178));
   pixels.setPixelColor(34, pixels.Color(23,33,178));
   pixels.setPixelColor(35, pixels.Color(23,33,178));
   pixels.setPixelColor(36, pixels.Color(23,33,178));
   pixels.show();
}


//===============================
// Print an F to the LED matrix
//===============================
// This prints an F to the centre of the LED matrix to show that the 
// mode has been changed to Foil.
void ledFoil() {
   clearLEDs();
   pixels.setPixelColor( 2, pixels.Color(23,33,178));
   pixels.setPixelColor( 3, pixels.Color(23,33,178));
   pixels.setPixelColor( 4, pixels.Color(23,33,178));
   pixels.setPixelColor(10, pixels.Color(23,33,178));
   pixels.setPixelColor(18, pixels.Color(23,33,178));
   pixels.setPixelColor(19, pixels.Color(23,33,178));
   pixels.setPixelColor(26, pixels.Color(23,33,178));
   pixels.setPixelColor(34, pixels.Color(23,33,178));
   pixels.show();
}


//===============================
// Print an S to the LED matrix
//===============================
// This prints an S to the centre of the LED matrix to show that the 
// mode has been changed to Sabre.
void ledSabre() {
   clearLEDs();
   pixels.setPixelColor( 2, pixels.Color(23,33,178));
   pixels.setPixelColor( 3, pixels.Color(23,33,178));
   pixels.setPixelColor( 4, pixels.Color(23,33,178));
   pixels.setPixelColor(10, pixels.Color(23,33,178));
   pixels.setPixelColor(18, pixels.Color(23,33,178));
   pixels.setPixelColor(19, pixels.Color(23,33,178));
   pixels.setPixelColor(20, pixels.Color(23,33,178));
   pixels.setPixelColor(28, pixels.Color(23,33,178));
   pixels.setPixelColor(34, pixels.Color(23,33,178));
   pixels.setPixelColor(35, pixels.Color(23,33,178));
   pixels.setPixelColor(36, pixels.Color(23,33,178));
   pixels.show();
}


//==================
// Show green LEDs
//==================
// Print a red rectangle to show an on target hit
void ledOnTargRed() {
   // turn on single led output
   digitalWrite(onTargetRed, HIGH);
   // setup led matrix to signal hit
   for(int i=4;i<8;i++){
      for(int j=0;j<9;j++){
         pixels.setPixelColor(j*8+i, pixels.Color(0,50,0));
      }
   }
   pixels.show();
}


//============================================
// Show red LEDs
//============================================
// Print a green rectangle to show an on target hit
void ledOnTargGrn() {
   // turn on single led output
   digitalWrite(onTargetGrn, HIGH);
   // setup led matrix to signal hit
   for(int i=0;i<4;i++){
      for(int j=0;j<9;j++){
         pixels.setPixelColor(j*8+i, pixels.Color(50,0,0));
      }
   }
   pixels.show();
}


//==================
// Show white LEDs
//==================
// Print a white rectangle to show a red off target hit
void ledOffTargRed() {
   // turn on single led output
   digitalWrite(offTargetRed, HIGH);
   // setup led matrix to signal hit
   for(int i=5;i<6;i++){
      for(int j=0;j<9;j++){
        pixels.setPixelColor(j*8+i, pixels.Color(50,50,50));
      }
   }
   pixels.setPixelColor(14, pixels.Color(50,50,50));
   pixels.setPixelColor(22, pixels.Color(50,50,50));
   pixels.setPixelColor(23, pixels.Color(50,50,50));
   pixels.setPixelColor(30, pixels.Color(50,50,50));
   pixels.show();
}


//==================
// Show white LEDs
//==================
// Print a white rectangle to show a green off target hit
void ledOffTargGrn() {
   // turn on single led output
   digitalWrite(offTargetGrn, HIGH);
   // setup led matrix to signal hit
   for(int i=2;i<3;i++){
      for(int j=0;j<9;j++){
         pixels.setPixelColor(j*8+i, pixels.Color(50,50,50));
      }
   }
   pixels.setPixelColor( 9, pixels.Color(50,50,50));
   pixels.setPixelColor(16, pixels.Color(50,50,50));
   pixels.setPixelColor(17, pixels.Color(50,50,50));
   pixels.setPixelColor(25, pixels.Color(50,50,50));
   pixels.show();
}


//===================
// Show yellow LEDs
//===================
// Print a yellow square to show a red short circuit
void ledShortRed() {
   for(int i=6;i<8;i++){
      for(int j=3;j<5;j++){
         pixels.setPixelColor(j*8+i, pixels.Color(100,50,0));
      }
   }
   pixels.show();
}


//===================
// Show yellow LEDs
//===================
// Print a yellow square to show a grn short circuit
void ledShortGrn() {
   for(int i=0;i<2;i++){
      for(int j=3;j<5;j++){
         pixels.setPixelColor(j*8+i, pixels.Color(100,50,0));
     }
   }
   pixels.show();
}