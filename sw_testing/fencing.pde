/*
fencing box ver 0.1

This code implements a fencing scoring box.
It is capable of running 3 different modes, namely foil, epee and sabre.
More to follow...

*/

#include <avr/interrupt.h>
#include <avr/io.h>

// global variables
int mode = 0;

// pin number constants
const int MODEBUTTON = 0;
const int FENCER1    = 1;
const int FENCER2    = 2;
const int PIN3       = 3;
const int PIN4       = 4;
const int PIN5       = 5;
const int PIN6       = 6;
const int PIN7       = 7;
const int PIN8       = 8;
const int PIN9       = 9;
const int PIN10      = 10;
const int PIN11      = 11;
const int REDPIN     = 12;
const int GRNPIN     = 13;
const int YEL1PIN    = 14;
const int YEL2PIN    = 15;

// mode constants
const int FOIL_MODE  = 0;
const int EPEE_MODE  = 1;
const int SABRE_MODE = 2;

// timing constants
const int FOILLOCKOUTTIME   = 0;  // 300.048 ms
const int EPEELOCKOUTTIME   = 0;  // 47.88   ms
const int SABRELOCKOUTTIME  = 0;  // 121.296 ms

const int FOILDEBOUNCETIME  = 0;  // 13.965  ms
const int EPEEDEBOUNCETIME  = 0;  // 3.9     ms
const int SABREDEBOUNCETIME = 0;  // 0.798   ms

const int WHIPOVER          = 0;  // 15.162  ms
const int WHIPOVERWINDOW    = 0;  // 3.99    ms
const int WHIPOVERLOCKOUT   = 0;  // 15.162  ms



// foil mode setup
void foilSetup() {
  attachInterrupt(FENCER1, foilFencer1OnTarget,  RISING);
  attachInterrupt(FENCER1, foilFencer1OffTarget, FALLING);
  attachInterrupt(FENCER2, foilFencer2OnTarget,  RISING);
  attachInterrupt(FENCER2, foilFencer2OffTarget, FALLING);
}

// epee mode setup
void epeeSetup() {
  attachInterrupt(FENCER1, epeeFencer1OnTarget, RISING);
  attachInterrupt(FENCER2, epeeFencer2OnTarget, RISING);
}

// sabre mode setup
void sabreSetup() {
  attachInterrupt(FENCER1, sabreFencer1OnTarget, RISING);
  attachInterrupt(FENCER2, sabreFencer2OnTarget, RISING);
}

// general setup, gets run on start up and on mode change
void setup() {
  attachInterrupt(MODEBUTTON, changeMode, RISING);
  attachInterrupt(FENCER1,    hitFencer1, CHANGE);
  attachInterrupt(FENCER2,    hitFencer2, CHANGE);
  pinMode(MODEBUTTON, INPUT);
  pinMode(REDPIN,     OUTPUT);
  pinMode(GRNPIN,     OUTPUT);
  pinMode(YEL1PIN,    OUTPUT);
  pinMode(YEL2PIN,    OUTPUT);
  testLights();
  if (mode == FOIL_MODE)
     foilSetup();
  if (mode == EPEE_MODE)
     epeeSetup();
  if (mode == SABRE_MODE)
     sabreSetup();
}

// flahses all the lights for 1 second when the box is turned on or reset
// run in the setup
void testLights() {
  digitalWrite(REDPIN, HIGH);
  digitalWrite(GRNPIN, HIGH);
  digitalWrite(YEL1PIN, HIGH);
  digitalWrite(YEL2PIN, HIGH);
  delay(500);
  digitalWrite(REDPIN, LOW);
  digitalWrite(GRNPIN, LOW);
  digitalWrite(YEL1PIN, LOW);
  digitalWrite(YEL2PIN, LOW);
}

void loop() {
  if (mode == FOIL_MODE)
    foil();
  if (mode == EPEE_MODE)
    epee();
  if (mode == SABRE_MODE)
    sabre();
}

// when the mode button is pressed this method is run
void changeMode() {

  if (mode == 2)
    mode = 0;
  else
    mode++;
  setup();
}

void foil() {
  Serial.println("Foil");
  // foil logic code here
}

void epee() {
  Serial.println("Epee");
  // epee logic code here
}

void sabre() {
  Serial.println("Sabre");
  // sabre logic code here
}



void hitFencer1onTar() {
  digitalWrite(REDPIN, HIGH);
}

void hitFencer1offTar() {
  digitalWrite(YEL1PIN, HIGH);
}

void hitFencer2onTar() {
  digitalWrite(GRNPIN, HIGH);
}

void hitFencer2offTar() {
  digitalWrite(YEL2PIN, HIGH);
}



// FOIL

void foilFencer1OnTarget() {

}

void foilFencer1OffTarget() {

}

void foilFencer2OnTarget() {

}

void foilFencer2OffTarget() {

}

// EPEE

void epeeFencer1OnTarget() {

}

void epeeFencer2OnTarget() {

}

// SABRE

void sabreFencer1OnTarget() {

}

void sabreFencer2OnTarget() {

}

