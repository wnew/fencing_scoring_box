//===========================================================================//
//                                                                           //
//  Desc: Arduino Code to implement a foil scoring apparatus                 //
//  Dev:  DigitalWestie & Wnew                                               //
//  Date: Nov 2012                                                           //
//  Notes: Origonal code from digitalwestie on github                        //
//         Plan to edit to include other weapons                             //
//                                                                           //
//===========================================================================//

//===========
// #defines
//===========
#define ON   256
#define OFF    0
#define HALF 128

int offTargetA = 2;         // Off Target A Light
int offTargetB = 1;         // Off Target B Light
int onTargetA  = 3;         // On Target A Light
int onTargetB  = 0;         // On Target B Light

int weaponPinA = 12;        // Weapon A pin
int weaponPinB = 11;        // Weapon B pin
int lamePinA   = 10;        // Lame A pin
int lamePinB   = 9;         // Lame B pin

int onA  = 0;
int onB  = 0;
int offA = 0;
int offB = 0;

char input = ' ';

int voltageThresh = 500;         // the threshold that the scoring triggers on

void setup() {
   pinMode(weaponPinA, OUTPUT);
   pinMode(weaponPinB, OUTPUT);
   pinMode(lamePinA,   OUTPUT);
   pinMode(lamePinB,   OUTPUT);

   Serial.begin(9600);
}

void loop()
{
   menu();
}

void menu()
{
   Serial.println("Welcome to the Fencing Scoring Machine tester");
   Serial.println("==============================================");
   Serial.println("Select Weapon:");
   Serial.println("1. Foil");
   Serial.println("2. Epee");
   Serial.println("3. Sabre");
   while(!Serial.available());
   input = Serial.read();
   if (input == '1')
   {
      // foil menu
      Serial.println("Select Foil Test:");
      Serial.println("1. Get foil lockout time");
      Serial.println("2. Get foil depress time");
      Serial.println("3. Run all foil senarios");
      while(!Serial.available());
      input = Serial.read();
      if (input == '1')
      {
         foil_get_lockout(0);
      }
      if (input == '2')
      {
         foil_get_depress(0);
      }
      if (input == '3')
      {
         foil_run_all_senarios();
      }
   }
   if (input == '2')
   {
      // epee menu
      Serial.println("Select Epee Test:");
      Serial.println("1. Get epee lockout time");
      Serial.println("2. Get epee depress time");
      Serial.println("3. Run all epee senarios");
      while(!Serial.available());
      input = Serial.read();
      if (input == '1')
      {
         epee_get_lockout(0);
      }
      if (input == '2')
      {
         epee_get_depress(0);
      }
      if (input == '3')
      {
         epee_run_all_senarios();
      }
   }
   if (input == '3')
   {
      // sabre menu
      Serial.println("Select Sabre Test:");
      Serial.println("1. Get sabre lockout time");
      Serial.println("2. Run all sabre senarios");
      while(!Serial.available());
      input = Serial.read();
      if (input == '1')
      {
         //sabre_get_timing(0);
      }
      if (input == '2')
      {
         //sabre_all_senarios();
      }
   }
}


void foil_run_all_senarios()
{
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


void foil_get_lockout(int lockout)
{
   Serial.println("finding foil lockout time");
   for (int i = 295; i < 305; i++)
   {
      Serial.print(i);
      Serial.println("ms");
      analogWrite(weaponPinA, HALF);
      analogWrite(lamePinB,   HALF);
      delay(i);
      analogWrite(weaponPinB, HALF);
      analogWrite(lamePinA,   HALF);
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

void foil_get_depress(int depress)
{
   Serial.println("finding foil depress time");
   for (int i = 10; i < 20; i++)
   {
      Serial.print(i);
      Serial.println("ms");
      analogWrite(weaponPinA, HALF);
      analogWrite(lamePinB,   HALF);
      delay(i);
      analogWrite(weaponPinA, ON);
      analogWrite(lamePinB,   OFF);
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
void foil_test_hit()
{
   Serial.println("A on 10ms released 10ms B on 10ms released");
   analogWrite(weaponPinA, HALF);
   analogWrite(lamePinB,   HALF);
   delay(10);
   analogWrite(weaponPinA, ON);
   analogWrite(lamePinB,   OFF);
   delay(10);
   analogWrite(weaponPinB, HALF);
   analogWrite(lamePinA,   HALF);
   delay(10);
   analogWrite(weaponPinB, ON);
   analogWrite(lamePinA,   OFF);
   foil_reset();
   read_lights();
   if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}



// fencer A ontarget for more then the depressed time
void foil_hitA()
{
   Serial.println("foil_hitA");
   analogWrite(weaponPinA, HALF);
   analogWrite(lamePinB,   HALF);
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
void foil_hitB()
{
   Serial.println("foil hitB");
   analogWrite(weaponPinB, HALF);
   analogWrite(lamePinA,   HALF);
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
void foil_off_targetA()
{
   Serial.println("foil_offtargetA");
   analogWrite(weaponPinA, OFF);
   delay(50);
   foil_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA > 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B ontarget for more then the depressed time
void foil_off_targetB()
{
   Serial.println("foil offtargetB");
   analogWrite(weaponPinB, OFF);
   delay(50);
   foil_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB > 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A ontarget for less then the depressed time
void foil_short_hitA()
{
   Serial.println("foil_short_hitA no delay ???");
   analogWrite(weaponPinA, HALF);
   analogWrite(lamePinB,   HALF);
   foil_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B ontarget for less then the depressed time
void foil_short_hitB()
{
   Serial.println("foil_short_hitB no delay ???");
   analogWrite(weaponPinB, HALF);
   analogWrite(lamePinA,   HALF);
   foil_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A hits the piste
void foil_fA_piste()
{
   Serial.println("foil_fA_piste");
   analogWrite(weaponPinA, OFF);
   foil_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B hits the piste
void foil_fB_piste()
{
   Serial.println("foil_fB_piste");
   analogWrite(weaponPinB, OFF);
   foil_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A on target then fencer B on target before lockout
void foil_hitA_hitB()
{
   Serial.println("foil_hitA_hitB with 40ms delay");
   analogWrite(weaponPinA, HALF);
   analogWrite(lamePinB,   HALF);
   delay(40);
   analogWrite(weaponPinB, HALF);
   analogWrite(lamePinA,   HALF);
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
void foil_hitB_hitA()
{
   Serial.println("foil_hitB_hitA with 40ms delay");
   analogWrite(weaponPinB, HALF);
   analogWrite(lamePinA,   HALF);
   delay(40);
   analogWrite(weaponPinA, HALF);
   analogWrite(lamePinB,   HALF);
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
void foil_hitA_off_targetB()
{
   Serial.println("foil_hitA_off_targetB with 280ms delay");
   analogWrite(weaponPinA, HALF);
   analogWrite(lamePinB,   HALF);
   delay(280);
   analogWrite(weaponPinB, OFF);
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
void foil_hitB_off_targetA()
{
   Serial.println("foil_hitB_off_targetA with 280ms delay");
   analogWrite(weaponPinB, HALF);
   analogWrite(lamePinA,   HALF);
   delay(280);
   analogWrite(weaponPinA, OFF);
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
void foil_off_targetA_hitB()
{
   Serial.println("foil_off_targetA_hitB with 280ms delay");
   analogWrite(weaponPinA, OFF);
   delay(280);
   analogWrite(weaponPinB, HALF);
   analogWrite(lamePinA,   HALF);
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
void foil_off_targetB_hitA()
{
   Serial.println("foil_off_targetB_hitA with 280ms delay");
   analogWrite(weaponPinB, OFF);
   delay(280);
   analogWrite(weaponPinA, HALF);
   analogWrite(lamePinB,   HALF);
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
void foil_hitA_lockout_off_targetB()
{
   Serial.println("foil_hitA_lockout_off_targetB with 320ms delay");
   analogWrite(weaponPinA, HALF);
   analogWrite(lamePinB,   HALF);
   delay(320);
   analogWrite(weaponPinB, OFF);
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
void foil_hitB_lockout_off_targetA()
{
   Serial.println("foil_hitB_lockout_off_targetA with 320ms delay");
   analogWrite(weaponPinB, ON);
   analogWrite(lamePinA, ON);
   delay(320);
   analogWrite(weaponPinA, ON);
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
void foil_hitA_lockout_hitB()
{
   Serial.println("foil_hitA_lockout_hitB 320ms delay");
   analogWrite(weaponPinA, ON);
   analogWrite(lamePinB, ON);
   delay(320);
   analogWrite(weaponPinB, ON);
   analogWrite(lamePinA, ON);
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
void foil_hitB_lockout_hitA()
{
   Serial.println("foil_hitB_lockout_hitA 320ms delay");
   analogWrite(weaponPinB, ON);
   analogWrite(lamePinA, ON);
   delay(320);
   analogWrite(weaponPinA, ON);
   analogWrite(lamePinB, ON);
   delay(100);
   foil_reset();
   read_lights();
   if (onA < 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}




void epee_run_all_senarios()
{
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
void generic_Epee_hit(bool a_hit_first, int time_between_hits, bool hitB)
{
   if (a_hit_first)
      analogWrite(weaponPinA, OFF);
   else
      analogWrite(weaponPinB, OFF);
   delay(time_between_hits);
   if (hitB && a_hit_first)
      analogWrite(weaponPinB, OFF);
   if (hitB && !a_hit_first)
      analogWrite(weaponPinA, OFF);
   delay(100);
   analogWrite(weaponPinA, ON);
   analogWrite(weaponPinB, ON);
   read_lights();
}

void epee_get_lockout(int lockout)
{
   Serial.println("finding epee lockout time");
   for (int i = 40; i < 60; i++)
   {
      Serial.print(i);
      Serial.println("ms");
      analogWrite(weaponPinA, OFF);
      analogWrite(lamePinA, ON);
      delay(i);
      analogWrite(weaponPinB, OFF);
      analogWrite(lamePinB, ON);
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

void epee_get_depress(int depress)
{
   Serial.println("finding epee depress time");
   for (int i = 1; i < 10; i++)
   {
      Serial.print(i);
      Serial.println("ms");
      analogWrite(weaponPinA, OFF);
      analogWrite(lamePinA, ON);
      delay(i);
      analogWrite(weaponPinA, ON);
      analogWrite(lamePinA, OFF);
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
void epee_hitA()
{
   Serial.println("epee_hitA");
   analogWrite(weaponPinA, OFF);
   analogWrite(lamePinA, ON);
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
void epee_hitB()
{
   Serial.println("epee hitB");
   analogWrite(weaponPinB, OFF);
   analogWrite(lamePinB, ON);
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
void epee_short_hitA()
{
   Serial.println("epee_short_hitA no delay ???");
   analogWrite(weaponPinA, OFF);
   analogWrite(lamePinA, ON);
   epee_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer B ontarget for less then the depressed time
void epee_short_hitB()
{
   Serial.println("epee_short_hitB no delay ???");
   analogWrite(weaponPinB, OFF);
   analogWrite(lamePinB, ON);
   epee_reset();
   read_lights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer A hits the piste
void epee_fA_piste()
{
   Serial.println("epee_fA_piste");
   analogWrite(weaponPinA, OFF);
   analogWrite(lamePinA, OFF);
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
void epee_fB_piste()
{
   Serial.println("epee_fB_piste");
   analogWrite(weaponPinB, OFF);
   analogWrite(lamePinB, OFF);
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
void epee_hitA_hitB()
{
   Serial.println("epee_hitA_hitB with 40ms delay");
   analogWrite(weaponPinA, OFF);
   analogWrite(lamePinA, ON);
   delay(40);
   analogWrite(weaponPinB, OFF);
   analogWrite(lamePinB, ON);
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
void epee_hitB_hitA()
{
   Serial.println("epee_hitB_hitA with 40ms delay");
   analogWrite(weaponPinB, OFF);
   analogWrite(lamePinB, ON);
   delay(40);
   analogWrite(weaponPinA, OFF);
   analogWrite(lamePinA, ON);
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
void epee_hitA_lockout_hitB()
{
   Serial.println("epee_hitA_lockout_hitB 50ms delay");
   analogWrite(weaponPinA, OFF);
   analogWrite(lamePinA, ON);
   delay(50);
   analogWrite(weaponPinB, OFF);
   analogWrite(lamePinB, ON);
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
void epee_hitB_lockout_hitA()
{
   Serial.println("epee_hitB_lockout_hitA 50ms delay");
   analogWrite(weaponPinB, OFF);
   analogWrite(lamePinB, ON);
   delay(50);
   analogWrite(weaponPinA, OFF);
   analogWrite(lamePinA, ON);
   delay(100);
   epee_reset();
   read_lights();
   if (onA < 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}


void read_lights()
{
   delay(1000);
   offA = analogRead(offTargetA);
   offB = analogRead(offTargetB);
   onA  = analogRead(onTargetA);
   onB  = analogRead(onTargetB);
}


void foil_reset()
{
   analogWrite(weaponPinA, OFF);
   analogWrite(weaponPinB, OFF);
   analogWrite(lamePinA,   OFF);
   analogWrite(lamePinB,   OFF);
   delay(100);
}

void epee_reset()
{
   analogWrite(weaponPinA, ON);
   analogWrite(weaponPinB, ON);
   analogWrite(lamePinA,   OFF);
   analogWrite(lamePinB,   OFF);
   delay(100);
}
