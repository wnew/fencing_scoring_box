//===========================================================================//
//                                                                           //
//  Desc:    Arduino Code to implement a fencing epee buzz box               //
//  Dev:     Wnew                                                            //
//  Date:    Nov  2012                                                       //
//  Updated: Jul  2024                                                       //
//  Notes:   1. This compiles, but will need to be tested                    //
//                                                                           //
//  To do:   1. Test on a circuit (This code has just been modified from the //
//              all weapons box)                                             //
//                                                                           //
//===========================================================================//

// The pins of the connectors are referred to as A-B--C
// Where A is: the return path for Epee, the Lame for Foil and Sabre
// Where B is: live for all three weapons, it connects to the blade
// Where C is: connected to the guard of all 3 weapons and is grounded 
// In this code the order of the weapons alphabetical, epee, foil sabre.

//============
// #defines
//============
#define DEBUG                // prints debug info to the serial terminal
#define BUZZERTIME   500     // length of time the buzzer is kept on after a hit (ms)
#define LIGHTTIME   3000     // length of time the lights are kept on after a hit (ms)
#define BAUDRATE   57600     // baudrate of the serial debug interface

//=========================================
// Pin Setup PCB ver 5, Arduino Nano
//=========================================
// LED pins
const uint8_t onTarget  =  7;    // On Target  Light

// Fencer pins
const uint8_t lamePin   = A0;    // Lame   pin (A) - Analog (Epee return path)
const uint8_t weaponPin = A1;    // Weapon pin (B) - Analog
const uint8_t groundPin = A2;    // Ground pin (C) - Analog

// buzzer pins
const uint8_t buzzerPin       =  3; // buzzer pin

//=================================
// initial values of analog reads
//=================================
int A = 0;
int B = 0;
int C = 0;

//=======================
// depress and timeouts
//=======================
long depressTime = 0;

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
//                        epee
const long depress [] = { 2000};  // the minimum amount of time the tip needs to be depressed

//=========
// states
//=========
boolean depressed  = false;
boolean hitOnTarg = false;

//================
// Configuration
//================
void setup() {
#ifdef DEBUG
   Serial.begin(BAUDRATE);  // start serial connection
   delay(200);              // wait a short time to ensure the serial is up before writing to it
   Serial.println("Epee Buzz Box");
   Serial.println("====================");
#endif

   // set the light pins to outputs
   pinMode(onTarget,  OUTPUT);
   pinMode(buzzerPin,  OUTPUT);
}


//============
// Main Loop
//============
void loop() {
   // use a while as a main loop as the loop() has too much overhead for fast analogReads
   // we get a 3-4% speed up on the loop this way
   while(1) {
      // read analog pins
      A = analogRead(lamePin);
      B = analogRead(weaponPin);
      signalHits();
      epee();
   }
}


// the following variables are unsigned long's because the time, measured in milliseconds,
// will quickly overflow an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 250;   // the debounce time; increase if the receiving multiple interrupts per button press


//===================
// Main epee method
//===================
void epee() {
   long now = micros();

   // weapon 
   //  no hit for yet    && weapon depress
   if (hitOnTarg == false) {
      if (400 < B && B < 600 && 400 < A && A < 600) {
         if (!depressed) {
            depressTime = micros();
            depressed   = true;
         } else {
            if (depressTime + depress[0] <= micros()) {
               hitOnTarg = true;
            }
         }
      } else {
         // reset these values if the depress time is short.
         if (depressed == true) {
            depressTime = 0;
            depressed   = 0;
         }
      }
   }
}


bool hitOnTargSignaled  = false;

//==============
// Signal Hits
//==============
void signalHits() {
   // this is run after a hit has been detected
   // turn on the buzzer if a hit has been registered
   if (hitOnTarg) {
      digitalWrite(buzzerPin, HIGH);
      resetValues();
   }
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

   depressTime = 0;
   depressed   = false;
   hitOnTarg   = false;
   hitOnTargSignaled  = false;
}
