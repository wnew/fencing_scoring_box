//===========================================================================//
//                                                                           //
//  Desc:    Arduino Code to implement a fencing scoring apparatus           //
//  Dev:     Wnew                                                            //
//  Date:    Nov  2012                                                       //
//  Updated: Aug  2024                                                       //
//                                                                           //
//  To do:   1. Implement whip-over for sabre                                //
//                                                                           //
//===========================================================================//

// Fencers are referred to as Green and Red. Green on the left and red on the right.
// The pins of the connectors are referred to as A-B--C
// Where A is: the return path for Epee, the Lame for Foil and Sabre
// Where B is: live for all three weapons, it connects to the blade
// Where C is: connected to the guard of all 3 weapons and is grounded 
// In this code the order of the weapons alphabetical, epee, foil sabre.

//============
// #includes
//============
#include <Adafruit_NeoPixel.h>  // adafruit's library for neopixel displays
#include <arduino-timer.h>      // timer for scheduling the short circuit checker

//============
// #defines
//============
#define DEBUG                // prints debug info to the serial terminal
#define TEST_LIGHTS          // turns on lights for a second on start up
#define NEOPIXELS            // if this is set then sketch uses the neopixel display, if not then individual leds per pin are assumed.
#define BUZZERSOUNDON        // buzzer will only sound if this is defined
#define BUZZERTIME   500     // length of time the buzzer is kept on after a hit (ms)
#define LIGHTTIME   3000     // length of time the lights are kept on after a hit (ms)
#define BAUDRATE   57600     // baudrate of the serial debug interface

#define LED_MATRIX_PIN    13 // neopixels data pin
#define NUMPIXELS         64 // number of NeoPixels on display
#define MATRIX_BRIGHTNESS  1 // 1-5, 1 being the dimmest and 5 the brightest, anything above 1 please ensure you have
                             // a good power supply otherwise the board can brown out and hang.
#define INITIAL_WEAPON     0 // define the weapon which is active on startup, 0 - epee, 1 - foil, 2 - sabre

// initialise the neopixel to control the led matrix
Adafruit_NeoPixel matrix = Adafruit_NeoPixel(NUMPIXELS, LED_MATRIX_PIN, NEO_GRB + NEO_KHZ800);
// initialise the short circuit check timer
auto short_circuit_timer = timer_create_default();

//=========================================
// Pin Setup PCB ver 5, Arduino Nano
//=========================================
// LED pins
const uint8_t onTargetGrn  =  7;    // On Target Grn Light
const uint8_t offTargetGrn =  8;    // Off Target Grn Light
const uint8_t shortLEDGrn  =  9;    // Short Circuit Grn Light
const uint8_t shortLEDRed  = 10;    // Short Circuit Red Light
const uint8_t offTargetRed = 11;    // Off Target Red Light
const uint8_t onTargetRed  = 12;    // On Target Red Light

// Fencer pins
const uint8_t groundPinGrn = A4;    // Grn Ground pin (C) - Analog
const uint8_t weaponPinGrn = A2;    // Grn Weapon pin (B) - Analog
const uint8_t lamePinGrn   = A0;    // Grn Lame   pin (A) - Analog (Epee return path)
const uint8_t lamePinRed   = A1;    // Red Lame   pin (A) - Analog (Epee return path)
const uint8_t weaponPinRed = A3;    // Red Weapon pin (B) - Analog
const uint8_t groundPinRed = A5;    // Red Ground pin (C) - Analog

// Button and buzzer pins
const uint8_t weaponSelectPin = 2; // Weapon select button interrupt pin 0 (digital pin 2)
const uint8_t buzzerPin       = 3; // buzzer pin

// if you wish to save pins, the hit leds can be used to signal the weapon selected as follows.
// red on target LED for Epee
// grn on target LED for Foil
// both red and grn on target LEDs for Sabre
// otherwise set the correct pins here
const uint8_t weaponSelectLeds[] = {4, 5, 6}; // LED pins to indicate weapon is selected {e f s}

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
//                         epee   foil   sabre
const long lockout [] = {45000, 300000, 120000};  // the lockout time between hits
const long depress [] = { 2000,  14000,   1000};  // the minimum amount of time the tip needs to be depressed

//=======================
// weapon mode constants
//=======================
const uint8_t EPEE_MODE  = 0;
const uint8_t FOIL_MODE  = 1;
const uint8_t SABRE_MODE = 2;

// set the initial mode
uint8_t currentWeapon = INITIAL_WEAPON;

//=========
// states
//=========
boolean depressedGrn  = false;
boolean depressedRed  = false;
boolean hitOnTargGrn  = false;
boolean hitOffTargGrn = false;
boolean hitOnTargRed  = false;
boolean hitOffTargRed = false;


//================
// Configuration
//================
void setup() {
#ifdef DEBUG
   Serial.begin(BAUDRATE);  // start serial connection
   delay(200);              // wait a short time to ensure the serial is up before writing to it
   Serial.println("3 Weapon Scoring Box");
   Serial.println("====================");
   Serial.print  ("Weapon : ");
   Serial.println(currentWeapon);
#endif

   // set the internal pullup resistor on weaponSelectPin
   pinMode(weaponSelectPin, INPUT_PULLUP);

   // add the interrupt to the mode pin (pin 2 is interrupt0 on the Uno and interrupt1 on the Micro)
   // change to weaponSelectPin-2 for the Uno
   attachInterrupt(digitalPinToInterrupt(weaponSelectPin), changeMode, FALLING);

   // set the mode leds to outputs and turn the correct one on
   pinMode(weaponSelectLeds[EPEE_MODE],  OUTPUT);
   pinMode(weaponSelectLeds[FOIL_MODE],  OUTPUT);
   pinMode(weaponSelectLeds[SABRE_MODE], OUTPUT);
   digitalWrite(weaponSelectLeds[currentWeapon], HIGH);

   // set the light pins to outputs
   pinMode(offTargetGrn, OUTPUT);
   pinMode(offTargetRed, OUTPUT);
   pinMode(onTargetGrn,  OUTPUT);
   pinMode(onTargetRed,  OUTPUT);
   pinMode(shortLEDGrn,  OUTPUT);
   pinMode(shortLEDRed,  OUTPUT);

   // set up buzzer pin to an output
   pinMode(buzzerPin,  OUTPUT);
   
   // initialise the timer for the short circuit checks
   short_circuit_timer.every(2000, shortCircuitCheck);

   // initialise the LED display
   matrix.begin();

#ifdef TEST_LIGHTS
   testLights();
#endif

   setweaponSelectLeds();
}


//============
// Main Loop
//============
void loop() {
   // use a while as a main loop as the loop() has too much overhead for fast analogReads
   // we get a 3-4% speed up on the loop this way
   while(1) {
      // read analog pins
      grnA = analogRead(lamePinGrn);
      redA = analogRead(lamePinRed);
      grnB = analogRead(weaponPinGrn);
      redB = analogRead(weaponPinRed);
      grnC = analogRead(groundPinGrn);
      redC = analogRead(groundPinRed);
      signalHits();
      // decide which weapon is currently selected
      if      (currentWeapon == EPEE_MODE)
         epee();
      else if (currentWeapon == FOIL_MODE)
         foil();
      else if (currentWeapon == SABRE_MODE)
         sabre();
      short_circuit_timer.tick();
   }
}


// the following variables are unsigned long's because the time, measured in milliseconds,
// will quickly overflow an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 250;   // the debounce time; increase if the receiving multiple interrupts per button press

//======================================
// Weapon select pin interrupt function
//======================================
void changeMode() {
   noInterrupts();
   if ((millis() - lastDebounceTime) > debounceDelay && digitalRead(weaponSelectPin) == LOW) { // 
      lastDebounceTime = millis();
      if (currentWeapon == 2)
         currentWeapon = 0;
      else
         currentWeapon++;
//#ifdef DEBUG
      Serial.print("Weapon changed to: ");
      Serial.println(currentWeapon);
      setweaponSelectLeds();
//#endif
   }
   interrupts();
}


//=============================
// Sets the correct weapon led
//=============================
void setweaponSelectLeds() {
   if (currentWeapon == EPEE_MODE) {
      ledEpee();
      digitalWrite(onTargetRed, HIGH);
   } else {
      if (currentWeapon == FOIL_MODE) {
         ledFoil();
         digitalWrite(onTargetGrn, HIGH);
      } else {
         if (currentWeapon == SABRE_MODE){
            ledSabre();
            digitalWrite(onTargetRed, HIGH);
            digitalWrite(onTargetGrn, HIGH);
         }
      }
   }
   long now = millis();

   #ifdef BUZZERSOUNDON
      digitalWrite(buzzerPin, HIGH);
      while (millis() < now + 100) {} // delay to keep the buzzer on
      digitalWrite(buzzerPin, LOW);
   #endif

   while (millis() < now + 1000) {} // delay to keep the leds on for 1 second
   digitalWrite(onTargetGrn, LOW);
   digitalWrite(onTargetRed, LOW);
   clearLEDs();
}


void shortCircuitCheck() {
   // Cant check A-C shorts with the current setup, they are both pulled to
   // ground via 1k and so should already be at the same voltage
   // confirm standby voltages for each weapon.
   clearLEDs();
    // Check A-C and B-C for epee
   if (currentWeapon == EPEE_MODE) {
      if (abs(grnB - grnC) < 50) { // check grn fencer B-C short
        ledShortGrn();
      }
      if (abs(redB - redC) < 50) { // check red fencer B-C short
        ledShortRed();
      }
   // Check A-B and A-C for foil
   } else if (currentWeapon == FOIL_MODE) {
      if (abs(grnA - grnB) < 50) { // check grn fencer A-B short
        ledShortGrn();
      }
      if (abs(redA - redB) < 50) { // check red fencer A-B short
        ledShortRed();
      }
   // Check A-B, A-C and B-C for sabre
   } else if (currentWeapon == SABRE_MODE) {
      if (abs(grnA - grnB) < 50) { // check grn fencer A-B short
        ledShortGrn();
      }
      if (abs(redA - redB) < 50) { // check red fencer A-B short
        ledShortRed();
      }
      if (abs(grnB - grnC) < 50) { // check grn fencer B-C short
        ledShortGrn();
      }
      if (abs(redB - redC) < 50) { // check red fencer B-C short
        ledShortRed();
      }
   }

   // as A and C are both pulled low, they should be at the same voltage at
   // rest, so this code puts C into output mode and sets C high, then reads
   // A to see if it also goes high. If it goes high there is a short. It then
   // sets the pins back to output mode so that the ADCs can be read again.
   // at 16MHz this code takes about 480uS to complete, this means it is 
   // unlikely to interfere with hit detection. It is also only run once
   // every 5 seconds

   // set the C pins to outputs
   pinMode(groundPinGrn,  OUTPUT);
   pinMode(groundPinRed,  OUTPUT);
   // write a high to the C pins
   digitalWrite(groundPinGrn, HIGH);
   digitalWrite(groundPinRed, HIGH);
   // get the adc values of the A pins
   grnA = analogRead(lamePinGrn);
   redA = analogRead(lamePinRed);
   // check if the A pins respond to the high C pins
   if (grnA > 700) {
      ledShortGrn();
   }
   if (redA > 700) {
      ledShortRed();
   }
   // set the C pins back to inputs
   pinMode(groundPinGrn,  INPUT);
   pinMode(groundPinRed,  INPUT);
   // read the adcs again to remove the possible high value
   grnA = analogRead(lamePinGrn);
   redA = analogRead(lamePinRed);
}


//===================
// Main epee method
//===================
void epee() {
   long now = micros();
   if ((hitOnTargGrn && (depressGrnTime + lockout[EPEE_MODE] < now)) ||
       (hitOnTargRed && (depressRedTime + lockout[EPEE_MODE] < now))) {
      lockedOut = true;
   }

   // weapon Grn
   //  no hit for Grn yet    && weapon depress    && opponent lame touched
   if (hitOnTargGrn == false) {
      if (400 < grnB && grnB < 600 && 400 < grnA && grnA < 600) {
         if (!depressedGrn) {
            depressGrnTime = micros();
            depressedGrn   = true;
         } else {
            if (depressGrnTime + depress[EPEE_MODE] <= micros()) {
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
      if (400 < redB && redB < 600 && 400 < redA && redA < 600) {
         if (!depressedRed) {
            depressRedTime = micros();
            depressedRed   = true;
         } else {
            if (depressRedTime + depress[EPEE_MODE] <= micros()) {
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
// Main foil method
//===================
void foil() {

   long now = micros();
   if (((hitOnTargGrn || hitOffTargGrn) && (depressGrnTime + lockout[FOIL_MODE] < now)) || 
       ((hitOnTargRed || hitOffTargRed) && (depressRedTime + lockout[FOIL_MODE] < now))) {
      lockedOut = true;
   }

   // weapon Grn
   if (hitOnTargGrn == false && hitOffTargGrn == false) { // ignore if Grn has already hit
      // off target
      if (900 < grnB && redA < 100) {
         if (!depressedGrn) {
            depressGrnTime = micros();
            depressedGrn   = true;
         } else {
            if (depressGrnTime + depress[FOIL_MODE] <= micros()) {
               hitOffTargGrn = true;
            }
         }
      } else {
         // on target
         if (400 < grnB && grnB < 600 && 400 < redA && redA < 600) {
            if (!depressedGrn) {
               depressGrnTime = micros();
               depressedGrn   = true;
            } else {
               if (depressGrnTime + depress[FOIL_MODE] <= micros()) {
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
      if (900 < redB && grnA < 100) {
         if (!depressedRed) {
            depressRedTime = micros();
            depressedRed   = true;
         } else {
            if (depressRedTime + depress[FOIL_MODE] <= micros()) {
               hitOffTargRed = true;
            }
         }
      } else {
         // on target
         if (400 < redB && redB < 600 && 400 < grnA && grnA < 600) {
            if (!depressedRed) {
               depressRedTime = micros();
               depressedRed   = true;
            } else {
               if (depressRedTime + depress[FOIL_MODE] <= micros()) {
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
// Main sabre method
//===================
void sabre() {

   long now = micros();
   if (((hitOnTargGrn || hitOffTargGrn) && (depressGrnTime + lockout[SABRE_MODE] < now)) || 
       ((hitOnTargRed || hitOffTargRed) && (depressRedTime + lockout[SABRE_MODE] < now))) {
      lockedOut = true;
   }

   // weapon Grn
   if (hitOnTargGrn == false && hitOffTargGrn == false) { // ignore if Grn has already hit
      // on target
      if (400 < grnB && grnB < 600 && 400 < redA && redA < 600) {
         if (!depressedGrn) {
            depressGrnTime = micros();
            depressedGrn   = true;
         } else {
            if (depressGrnTime + depress[SABRE_MODE] <= micros()) {
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
      if (400 < redB && redB < 600 && 400 < grnA && grnA < 600) {
         if (!depressedRed) {
            depressRedTime = micros();
            depressedRed   = true;
         } else {
            if (depressRedTime + depress[SABRE_MODE] <= micros()) {
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


bool hitOnTargGrnSignaled  = false;
bool hitOffTargGrnSignaled = false;
bool hitOffTargRedSignaled = false;
bool hitOnTargRedSignaled  = false;

//==============
// Signal Hits
//==============
void signalHits() {
   // this is run after a hit has been detected
   // turn on the buzzer if a hit has been registered
   if (hitOnTargGrn || hitOnTargRed || hitOffTargGrn || hitOffTargRed) {
   #ifdef BUZZERSOUNDON
      digitalWrite(buzzerPin, HIGH);
   #endif

   }
   if (hitOnTargGrn && !hitOnTargGrnSignaled) {
      hitOnTargGrnSignaled = true;
      ledOnTargGrn();
   }
   if (hitOffTargGrn && !hitOffTargGrnSignaled) {
      hitOffTargGrnSignaled = true;
      ledOffTargGrn();
   }
   if (hitOffTargRed && !hitOffTargRedSignaled) {
      hitOffTargRedSignaled = true;
      ledOffTargRed();
   }
   if (hitOnTargRed && !hitOnTargRedSignaled) {
      hitOnTargRedSignaled = true;
      ledOnTargRed();
   }
   if (lockedOut) {
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
      serData = String("Red depressed time  : ") + depressRedTime  + "\n"
                     + "Grn depressed time : "  + depressGrnTime + "\n";
      Serial.println(serData);
   #endif
      resetValues();
   }
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
         matrix.setPixelColor(j*8+i, matrix.Color(0,0,0));
      }
   }
   matrix.show();
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
   while (millis() < now + BUZZERTIME) {}
   digitalWrite(buzzerPin,  LOW);
   // wait before turning off the leds
   while (millis() < now + LIGHTTIME-BUZZERTIME) {}
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
   hitOnTargGrnSignaled  = false;
   hitOffTargGrnSignaled = false;
   hitOffTargRedSignaled = false;
   hitOnTargRedSignaled  = false;
}


//==============
// Test lights
//==============
// This function can be run on start up to show that all the lights are working
void testLights() {
   // turn on grn on target
   delay(500);
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
// weapon has been changed to Epee.
void ledEpee() {
   clearLEDs();
   matrix.setPixelColor( 0, matrix.Color(5,6,42));
   matrix.setPixelColor( 1, matrix.Color(5,6,42));
   matrix.setPixelColor( 2, matrix.Color(5,6,42));
   matrix.setPixelColor( 3, matrix.Color(5,6,42));
   matrix.setPixelColor( 4, matrix.Color(5,6,42));
   matrix.setPixelColor( 6, matrix.Color(5,6,42));
   matrix.setPixelColor( 8, matrix.Color(5,6,42));
   matrix.setPixelColor(10, matrix.Color(5,6,42));

   matrix.setPixelColor(16, matrix.Color(5,6,42));
   matrix.setPixelColor(20, matrix.Color(5,6,42));
   matrix.setPixelColor(24, matrix.Color(5,6,42));

   matrix.show();
}


//===============================
// Print an F to the LED matrix
//===============================
// This prints an F to the centre of the LED matrix to show that the 
// weapon has been changed to Foil.
void ledFoil() {
   clearLEDs();
   matrix.setPixelColor( 0, matrix.Color(5,6,42));
   matrix.setPixelColor( 1, matrix.Color(5,6,42));
   matrix.setPixelColor( 2, matrix.Color(5,6,42));
   matrix.setPixelColor( 3, matrix.Color(5,6,42));
   matrix.setPixelColor( 4, matrix.Color(5,6,42));
   matrix.setPixelColor( 6, matrix.Color(5,6,42));
   matrix.setPixelColor( 8, matrix.Color(5,6,42));

   matrix.setPixelColor(16, matrix.Color(5,6,42));

   matrix.show();
}


//===============================
// Print an S to the LED matrix
//===============================
// This prints an S to the centre of the LED matrix to show that the 
// weapon has been changed to Sabre.
void ledSabre() {
   clearLEDs();
   matrix.setPixelColor( 0, matrix.Color(5,6,42));
   matrix.setPixelColor( 1, matrix.Color(5,6,42));
   matrix.setPixelColor( 2, matrix.Color(5,6,42));
   matrix.setPixelColor( 4, matrix.Color(5,6,42));
   matrix.setPixelColor( 6, matrix.Color(5,6,42));
   matrix.setPixelColor( 8, matrix.Color(5,6,42));
   matrix.setPixelColor(10, matrix.Color(5,6,42));
   matrix.setPixelColor(11, matrix.Color(5,6,42));

   matrix.setPixelColor(16, matrix.Color(5,6,42));
   matrix.setPixelColor(20, matrix.Color(5,6,42));
   matrix.setPixelColor(24, matrix.Color(5,6,42));

   matrix.show();
}


//=========================
// Set grn on target LEDs
//=========================
// Sets a group of grn LEDs to show a red on target hit
void ledOnTargGrn() {
   // turn on single led output
   digitalWrite(onTargetGrn, HIGH);
   // setup led matrix to signal hit
   for(int i=0;i<32;i++){
      matrix.setPixelColor(i, matrix.Color(0,25*MATRIX_BRIGHTNESS,0));
   }
   matrix.show();
}


//=========================
// Set red on target LEDs
//=========================
// Sets a group of red LEDs to show a red on target hit
void ledOnTargRed() {
   long now = micros();
   // turn on single led output
   digitalWrite(onTargetRed, HIGH);
   // setup led matrix to signal hit
   for(int i=32;i<64;i++){
      matrix.setPixelColor(i, matrix.Color(25*MATRIX_BRIGHTNESS,0,0));
   }
  matrix.show();
}


//==========================
// Set grn off target LEDs
//==========================
// Sets a group of white LEDs to show a grn off target hit
void ledOffTargGrn() {
   // turn on single led output
   digitalWrite(offTargetGrn, HIGH);
   // setup led matrix to signal hit
   for(int i=16;i<32;i++){
      matrix.setPixelColor(i, matrix.Color(25*MATRIX_BRIGHTNESS,25*MATRIX_BRIGHTNESS,25*MATRIX_BRIGHTNESS));
   }
   matrix.show();
}


//==========================
// Set red off target LEDs
//==========================
// Sets a group of white LEDs to show a red off target hit
void ledOffTargRed() {
   // turn on single led output
   digitalWrite(offTargetRed, HIGH);
   // setup led matrix to signal hit
   for(int i=32;i<48;i++){
      matrix.setPixelColor(i, matrix.Color(25*MATRIX_BRIGHTNESS,25*MATRIX_BRIGHTNESS,25*MATRIX_BRIGHTNESS));
   }
   matrix.show();
}


//=============================
// Set grn short circuit LEDs
//=============================
// Sets a single yellow led on the matrix to show a grn short circuit
void ledShortGrn() {
   matrix.setPixelColor(31, matrix.Color(50*MATRIX_BRIGHTNESS,25*MATRIX_BRIGHTNESS,0));
   matrix.show();
}


//=============================
// Set red short circuit LEDs
//=============================
// Sets a single yellow led on the matrix to show a red short circuit
void ledShortRed() {
   matrix.setPixelColor(44, matrix.Color(50*MATRIX_BRIGHTNESS,25*MATRIX_BRIGHTNESS,0));
   matrix.show();
}
