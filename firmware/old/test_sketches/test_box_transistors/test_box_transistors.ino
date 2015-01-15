//===========================================================================//
//                                                                           //
//  Desc: Arduino Code to implement a foil scoring apparatus                 //
//  Dev:  DigitalWestie & Wnew                                               //
//  Date: Nov 2012                                                           //
//  Notes: Origonal code from digitalwestie on github                        //
//         Plan to edit to include other weapons                             //
//                                                                           //
//===========================================================================//

const uint8_t offTargetA = 2;        // Off Target A Light
const uint8_t offTargetB = 1;        // Off Target B Light
const uint8_t onTargetA  = 3;        // On Target A Light
const uint8_t onTargetB  = 0;        // On Target B Light

const uint8_t wpnA_gnd   = 2;        // 
const uint8_t wpnB_gnd   = 3;        // 
const uint8_t wpnA_lameA = 4;        // 
const uint8_t wpnB_lameB = 5;        // 
const uint8_t wpnA_lameB = 6;        // 
const uint8_t wpnB_lameA = 7;        // 

int onA  = 0;
int onB  = 0;
int offA = 0;
int offB = 0;

char input = ' ';

int voltageThresh = 500;         // the threshold that the scoring triggers on

void setup() {
   pinMode(wpnA_gnd,   OUTPUT);
   pinMode(wpnB_gnd,   OUTPUT);
   pinMode(wpnA_lameA, OUTPUT);
   pinMode(wpnB_lameB, OUTPUT);
   pinMode(wpnA_lameB, OUTPUT);
   pinMode(wpnB_lameA, OUTPUT);

   Serial.begin(57600);
}

void loop() {
   menu();
}

void menu() {
   Serial.println("Welcome to the Fencing Scoring Machine tester");
   Serial.println("==============================================");
   Serial.println("Select Weapon:");
   Serial.println("1. Foil");
   Serial.println("2. Epee");
   Serial.println("3. Sabre");
   while(!Serial.available());
   input = Serial.read();
   if (input == '1') {
      // foil menu
      Serial.println("Select Foil Test:");
      Serial.println("1. Get foil lockout time");
      Serial.println("2. Get foil depress time");
      Serial.println("3. Run all foil senarios");
      while(!Serial.available());
      input = Serial.read();
      if (input == '1') {
         foil_get_lockout(0);
      }
      if (input == '2') {
         foil_get_depress(0);
      }
      if (input == '3') {
         foil_run_all_senarios();
      }
      if (input == '4') {
         foil_off_targetA();
      }
      if (input == '5') {
         foil_off_targetB();
      }
      if (input == '6') {
         foil_hitA();
      }
      if (input == '7') {
         foil_hitB();
      }
   }
   /*if (input == '2') {
      // epee menu
      Serial.println("Select Epee Test:");
      Serial.println("1. Get epee lockout time");
      Serial.println("2. Get epee depress time");
      Serial.println("3. Run all epee senarios");
      while(!Serial.available());
      input = Serial.read();
      if (input == '1') {
         epee_get_lockout(0);
      }
      if (input == '2') {
         epee_get_depress(0);
      }
      if (input == '3') {
         epee_run_all_senarios();
      }
   }
   if (input == '3') {
      // sabre menu
      Serial.println("Select Sabre Test:");
      Serial.println("1. Get sabre lockout time");
      Serial.println("2. Run all sabre senarios");
      while(!Serial.available());
      input = Serial.read();
      if (input == '1') {
         //sabre_get_timing(0);
      }
      if (input == '2') {
         //sabre_all_senarios();
      }
   }*/
}


void foil_run_all_senarios() {
   foil_test_hit();
   foil_hitA();
   foil_hitB();
   foil_off_targetA();
   foil_off_targetB();
   foil_short_hitA();
   foil_short_hitB();
   foil_fA_piste();
   foil_fB_piste();
   foil_hitA_hitB();
   foil_hitB_hitA();
   foil_hitA_off_targetB();
   foil_hitB_off_targetA();
   foil_off_targetA_hitB();
   foil_off_targetB_hitA();
   foil_hitA_lockout_off_targetB();
   foil_hitB_lockout_off_targetA();
   foil_hitA_lockout_hitB();
   foil_hitB_lockout_hitA();
}


void foil_get_lockout(int lockout) {
   Serial.println("finding foil lockout time");
   for (int i = 295; i < 305; i++) {
      Serial.print(i);
      Serial.println("ms");
      digitalWrite(wpnA_lameB, HIGH);
      digitalWrite(wpnA_gnd,   HIGH);
      delay(i);
      digitalWrite(wpnB_lameA, HIGH);
      digitalWrite(wpnB_gnd,   HIGH);
      delay(100);
      foil_reset();
      read_lights();
      if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
         Serial.println("Double Hit");
      else
         Serial.println("Single Hit");
      delay(3000);
   }
}

void foil_get_depress(int depress) {
   Serial.println("finding foil depress time");
   for (int i = 10; i < 20; i++) {
      Serial.print(i);
      Serial.println("ms");
      digitalWrite(wpnA_gnd,   HIGH);
      digitalWrite(wpnA_lameB, HIGH);
      delay(i);
      digitalWrite(wpnA_gnd,   LOW);
      digitalWrite(wpnA_lameB, LOW);
      delay(100);
      foil_reset();
      read_lights();
      if (onA > 500 && onB < 500 && offA < 500 && offB < 500)
         Serial.println("Hit");
      else
         Serial.println("No Hit");
      delay(3000);
   }
}


// hitA off and
void foil_test_hit() {
   Serial.println("A on 10ms released 10ms B on 10ms released");
   digitalWrite(wpnA_gnd,   HIGH);
   digitalWrite(wpnA_lameB, HIGH);
   delay(10);
   digitalWrite(wpnA_gnd,   LOW);
   digitalWrite(wpnA_lameB, LOW);
   delay(10);
   digitalWrite(wpnA_gnd,   HIGH);
   digitalWrite(wpnA_lameB, HIGH);
   delay(10);
   digitalWrite(wpnA_gnd,   LOW);
   digitalWrite(wpnA_lameB, LOW);
   foil_reset();
   read_lights();
   if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}



// fencer A ontarget for more then the depressed time
void foil_hitA() {
   Serial.println("foil_hitA");
   digitalWrite(wpnA_gnd,   HIGH);
   digitalWrite(wpnA_lameB, HIGH);
   delay(50);
   foil_reset();
   read_lights();
   if (onA > 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B ontarget for more then the depressed time
void foil_hitB() {
   Serial.println("foil hitB");
   digitalWrite(wpnB_gnd,   HIGH);
   digitalWrite(wpnB_lameA, HIGH);
   delay(50);
   foil_reset();
   read_lights();
   if (onA < 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A ontarget for more then the depressed time
void foil_off_targetA() {
   Serial.println("foil_offtargetA");
   digitalWrite(wpnA_gnd, HIGH);
   delay(5000);
   foil_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA > 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B ontarget for more then the depressed time
void foil_off_targetB() {
   Serial.println("foil offtargetB");
   digitalWrite(wpnB_gnd, HIGH);
   delay(5000);
   foil_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB > 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A ontarget for less then the depressed time
void foil_short_hitA() {
   Serial.println("foil_short_hitA no delay ???");
   digitalWrite(wpnA_gnd,   HIGH);
   digitalWrite(wpnA_lameB, HIGH);
   foil_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B ontarget for less then the depressed time
void foil_short_hitB() {
   Serial.println("foil_short_hitB no delay ???");
   digitalWrite(wpnB_gnd,   HIGH);
   digitalWrite(wpnB_lameA, HIGH);
   foil_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A hits the piste
void foil_fA_piste() {
   Serial.println("foil_fA_piste");

   foil_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B hits the piste
void foil_fB_piste() {
   Serial.println("foil_fB_piste");

   foil_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A on target then fencer B on target before lockout
void foil_hitA_hitB() {
   Serial.println("foil_hitA_hitB with 40ms delay");
   digitalWrite(wpnA_gnd,   HIGH);
   digitalWrite(wpnA_lameB, HIGH);
   delay(40);
   digitalWrite(wpnB_gnd,   HIGH);
   digitalWrite(wpnB_lameA, HIGH);
   delay(100);
   foil_reset();
   read_lights();
   if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B on target then fencer A on target before lockout
void foil_hitB_hitA() {
   Serial.println("foil_hitB_hitA with 40ms delay");
   digitalWrite(wpnB_gnd,   HIGH);
   digitalWrite(wpnB_lameA, HIGH);
   delay(40);
   digitalWrite(wpnA_gnd,   HIGH);
   digitalWrite(wpnA_lameB, HIGH);
   delay(100);
   foil_reset();
   read_lights();
   if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A on target then fencer B on target before lockout
void foil_hitA_off_targetB() {
   Serial.println("foil_hitA_off_targetB with 280ms delay");
   digitalWrite(wpnA_gnd,   HIGH);
   digitalWrite(wpnA_lameB, HIGH);
   delay(280);
   digitalWrite(wpnB_gnd,   HIGH);
   delay(100);
   foil_reset();
   read_lights();
   if (onA > 500 && onB < 500 && offA < 500 && offB > 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B on target then fencer A on target before lockout
void foil_hitB_off_targetA() {
   Serial.println("foil_hitB_off_targetA with 280ms delay");
   digitalWrite(wpnB_gnd,   HIGH);
   digitalWrite(wpnB_lameA, HIGH);
   delay(280);
   digitalWrite(wpnA_gnd, HIGH);
   delay(100);
   foil_reset();
   read_lights();
   if (onA < 500 && onB > 500 && offA > 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A on target then fencer B on target before lockout
void foil_off_targetA_hitB() {
   Serial.println("foil_off_targetA_hitB with 280ms delay");
   digitalWrite(wpnA_gnd, HIGH);
   delay(280);
   digitalWrite(wpnB_gnd,   HIGH);
   digitalWrite(wpnB_lameA, HIGH);
   delay(100);
   foil_reset();
   read_lights();
   if (onA < 500 && onB > 500 && offA > 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A on target then fencer B on target before lockout
void foil_off_targetB_hitA() {
   Serial.println("foil_off_targetB_hitA with 280ms delay");
   digitalWrite(wpnB_gnd, HIGH);
   delay(280);
   digitalWrite(wpnA_gnd,   HIGH);
   digitalWrite(wpnA_lameB, HIGH);
   delay(100);
   foil_reset();
   read_lights();
   if (onA > 500 && onB < 500 && offA < 500 && offB > 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A on target then fencer B on target before lockout
void foil_hitA_lockout_off_targetB() {
   Serial.println("foil_hitA_lockout_off_targetB with 320ms delay");
   digitalWrite(wpnA_gnd,   HIGH);
   digitalWrite(wpnA_lameB, HIGH);
   delay(320);
   digitalWrite(wpnB_gnd, HIGH);
   delay(100);
   foil_reset();
   read_lights();
   if (onA > 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B on target then fencer A on target before lockout
void foil_hitB_lockout_off_targetA() {
   Serial.println("foil_hitB_lockout_off_targetA with 320ms delay");
   digitalWrite(wpnB_gnd,   HIGH);
   digitalWrite(wpnB_lameA, HIGH);
   delay(320);
   digitalWrite(wpnA_gnd, HIGH);
   delay(100);
   foil_reset();
   read_lights();
   if (onA < 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A on target then fencer B on target after lockout
void foil_hitA_lockout_hitB() {
   Serial.println("foil_hitA_lockout_hitB 320ms delay");
   digitalWrite(wpnA_gnd,   HIGH);
   digitalWrite(wpnA_lameB, HIGH);
   delay(320);
   digitalWrite(wpnB_gnd,   HIGH);
   digitalWrite(wpnB_lameA, HIGH);
   delay(100);
   foil_reset();
   read_lights();
   if (onA > 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B on target then fencer A on target after lockout
void foil_hitB_lockout_hitA() {
   Serial.println("foil_hitB_lockout_hitA 320ms delay");
   digitalWrite(wpnB_gnd,   HIGH);
   digitalWrite(wpnB_lameA, HIGH);
   delay(320);
   digitalWrite(wpnA_gnd,   HIGH);
   digitalWrite(wpnA_lameB, HIGH);
   delay(100);
   foil_reset();
   read_lights();
   if (onA < 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}




/*void epee_run_all_senarios() {
   epee_reset();
   epee_hitA();
   epee_hitB();
   epee_short_hitA();
   epee_short_hitB();
   epee_fA_piste();
   epee_fB_piste();
   epee_hitA_hitB();
   epee_hitB_hitA();
   epee_hitA_lockout_hitB();
   epee_hitB_lockout_hitA();
}

// is a_first_hit true/false
// time_between_hits
// hitB true/false is there a 2nd hit?
void generic_Epee_hit(bool a_hit_first, int time_between_hits, bool hitB) {
   if (a_hit_first)
      digitalWrite(weaponPinA, LOW);
   else
      digitalWrite(weaponPinB, LOW);
   delay(time_between_hits);
   if (hitB && a_hit_first)
      digitalWrite(weaponPinB, LOW);
   if (hitB && !a_hit_first)
      digitalWrite(weaponPinA, LOW);
   delay(100);
   digitalWrite(weaponPinA, HIGH);
   digitalWrite(weaponPinB, HIGH);
   read_lights();
}

void epee_get_lockout(int lockout) {
   Serial.println("finding epee lockout time");
   for (int i = 40; i < 60; i++) {
      Serial.print(i);
      Serial.println("ms");
      digitalWrite(weaponPinA, LOW);
      digitalWrite(lamePinA, HIGH);
      delay(i);
      digitalWrite(weaponPinB, LOW);
      digitalWrite(lamePinB, HIGH);
      delay(100);
      epee_reset();
      read_lights();
      if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
         Serial.println("Double Hit");
      else
         Serial.println("Single Hit");
      delay(3000);
   }
}

void epee_get_depress(int depress) {
   Serial.println("finding epee depress time");
   for (int i = 1; i < 10; i++)
   {
      Serial.print(i);
      Serial.println("ms");
      digitalWrite(weaponPinA, LOW);
      digitalWrite(lamePinA, HIGH);
      delay(i);
      digitalWrite(weaponPinA, HIGH);
      digitalWrite(lamePinA, LOW);
      delay(100);
      epee_reset();
      read_lights();
      if (onA > 500 && onB < 500 && offA < 500 && offB < 500)
         Serial.println(" Hit");
      else
         Serial.println("No Hit");
      delay(3000);
   }
}

// fencer A ontarget for more then the depressed time
void epee_hitA() {
   Serial.println("epee_hitA");
   digitalWrite(weaponPinA, LOW);
   digitalWrite(lamePinA, HIGH);
   delay(50);
   epee_reset();
   read_lights();
   if (onA > 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer B ontarget for more then the depressed time
void epee_hitB() {
   Serial.println("epee hitB");
   digitalWrite(weaponPinB, LOW);
   digitalWrite(lamePinB, HIGH);
   delay(50);
   epee_reset();
   read_lights();
   if (onA < 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer A ontarget for less then the depressed time
void epee_short_hitA() {
   Serial.println("epee_short_hitA no delay ???");
   digitalWrite(weaponPinA, LOW);
   digitalWrite(lamePinA, HIGH);
   epee_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer B ontarget for less then the depressed time
void epee_short_hitB() {
   Serial.println("epee_short_hitB no delay ???");
   digitalWrite(weaponPinB, LOW);
   digitalWrite(lamePinB, HIGH);
   epee_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer A hits the piste
void epee_fA_piste() {
   Serial.println("epee_fA_piste");
   digitalWrite(weaponPinA, LOW);
   digitalWrite(lamePinA, LOW);
   delay(100);
   epee_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer B hits the piste
void epee_fB_piste() {
   Serial.println("epee_fB_piste");
   digitalWrite(weaponPinB, LOW);
   digitalWrite(lamePinB, LOW);
   delay(100);
   epee_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer A on target then fencer B on target before lockout
void epee_hitA_hitB() {
   Serial.println("epee_hitA_hitB with 40ms delay");
   digitalWrite(weaponPinA, LOW);
   digitalWrite(lamePinA, HIGH);
   delay(40);
   digitalWrite(weaponPinB, LOW);
   digitalWrite(lamePinB, HIGH);
   delay(100);
   epee_reset();
   read_lights();
   if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer B on target then fencer A on target before lockout
void epee_hitB_hitA() {
   Serial.println("epee_hitB_hitA with 40ms delay");
   digitalWrite(weaponPinB, LOW);
   digitalWrite(lamePinB, HIGH);
   delay(40);
   digitalWrite(weaponPinA, LOW);
   digitalWrite(lamePinA, HIGH);
   delay(100);
   epee_reset();
   read_lights();
   if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer A on target then fencer B on target after lockout
void epee_hitA_lockout_hitB() {
   Serial.println("epee_hitA_lockout_hitB 50ms delay");
   digitalWrite(weaponPinA, LOW);
   digitalWrite(lamePinA, HIGH);
   delay(50);
   digitalWrite(weaponPinB, LOW);
   digitalWrite(lamePinB, HIGH);
   delay(100);
   epee_reset();
   read_lights();
   if (onA > 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer B on target then fencer A on target after lockout
void epee_hitB_lockout_hitA() {
   Serial.println("epee_hitB_lockout_hitA 50ms delay");
   digitalWrite(weaponPinB, LOW);
   digitalWrite(lamePinB, HIGH);
   delay(50);
   digitalWrite(weaponPinA, LOW);
   digitalWrite(lamePinA, HIGH);
   delay(100);
   epee_reset();
   read_lights();
   if (onA < 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}*/


void read_lights() {
   delay(1000);
   offA = analogRead(offTargetA);
   offB = analogRead(offTargetB);
   onA  = analogRead(onTargetA);
   onB  = analogRead(onTargetB);
}

void foil_reset() {
   digitalWrite(wpnA_gnd,   LOW);
   digitalWrite(wpnB_gnd,   LOW);
   digitalWrite(wpnA_lameA, LOW);
   digitalWrite(wpnB_lameB, LOW);
   digitalWrite(wpnA_lameB, LOW);
   digitalWrite(wpnB_lameA, LOW);
   delay(100);
}

void epee_reset() {
   digitalWrite(wpnA_gnd,   LOW);
   digitalWrite(wpnB_gnd,   LOW);
   digitalWrite(wpnA_lameA, HIGH);
   digitalWrite(wpnB_lameB, HIGH);
   digitalWrite(wpnA_lameB, LOW);
   digitalWrite(wpnB_lameA, LOW);
   delay(100);
}
