//===========================================================================//
//                                                                           //
//  Desc: Arduino Code to a fencing scoring apparatus                        //
//  Dev:  Wnew                                                               //
//  Date: Feb 2013                                                           //
//  Notes: Version 2                                                         //
//         This version is implemented in a more generic way that allows     //
//         for the testing of any scoring box, by the toggling of            //
//         fets                                                              //
//                                                                           //
//===========================================================================//

int offTargetALight = 2;   // Off Target A Light
int offTargetBLight = 1;   // Off Target B Light
int onTargetALight  = 3;   // On Target A Light
int onTargetBLight  = 0;   // On Target B Light

int B1_C1 = 12;   // Foil closed circuit 1
int B2_C2 = 11;   // Foil closed circuit 2
int A2_B1 = 10;   // Foil lame touch 1
int A1_B2 = 9;    // Foil lame touch 2
int A1_B1 = 8;    // Epee open circuit 1
int A2_B2 = 7;    // Epee open circuit 2
int A1_C2 = 6;    // Epee guard touch 1
int A2_C1 = 5;    // Epee guard touch 2

int onA  = 0;
int onB  = 0;
int offA = 0;
int offB = 0;

char input = ' ';

int voltageThresh = 500;         // the threshold that the scoring triggers on

void setup() {
   pinMode(A1_B1, OUTPUT);
   pinMode(A2_B2, OUTPUT);
   pinMode(A1_C2, OUTPUT);
   pinMode(A2_C1, OUTPUT);
   pinMode(B1_C1, OUTPUT);
   pinMode(B2_C2, OUTPUT);
   pinMode(A2_B1, OUTPUT);
   pinMode(A1_B2, OUTPUT);

   Serial.begin(9600);
}

void foilSetup() {
   digitalWrite(B1_C1, HIGH);
   digitalWrite(B2_C2, HIGH);
   digitalWrite(A2_B1, LOW);
   digitalWrite(A1_B2, LOW);
}

void epeeSetup() {
   digitalWrite(A1_B1, LOW);
   digitalWrite(A2_B2, LOW);
   digitalWrite(A1_C2, LOW);
   digitalWrite(A2_C1, LOW);
}

void sabreSetup() {
   digitalWrite(B1_C1, HIGH);
   digitalWrite(B2_C2, HIGH);
   digitalWrite(A2_B1, LOW);
   digitalWrite(A1_B2, LOW);
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
   while (!Serial.available());
   input = Serial.read();
   if (input == '1') {
      foilSetup();
      // foil menu
      Serial.println("Select Foil Test:");
      Serial.println("1. Get foil lockout time");
      Serial.println("2. Get foil depress time");
      Serial.println("3. Run all foil senarios");
      while(!Serial.available());
      input = Serial.read();
      if (input == '1') {
         foilGetLockout(0);
      }
      if (input == '2') {
         foilGetDepress(0);
      }
      if (input == '3') {
         foilRunAllSenarios();
      }
      input = 0;
   }
   if (input == '2') {
      epeeSetup();
      // epee menu
      Serial.println("Select Epee Test:");
      Serial.println("1. Get epee lockout time");
      Serial.println("2. Get epee depress time");
      Serial.println("3. Run all epee senarios");
      while (!Serial.available());
      input = Serial.read();
      if (input == '1') {
         epeeGetLockout(0);
      }
      if (input == '2') {
         epeeGetDepress(0);
      }
      if (input == '3') {
         epeeRunAllSenarios();
      }
      input = 0;
   }
   if (input == '3') {
      sabreSetup();
      // sabre menu
      Serial.println("Select Sabre Test:");
      Serial.println("1. Get sabre lockout time");
      Serial.println("2. Run all sabre senarios");
      while (!Serial.available());
      input = Serial.read();
      if (input == '1') {
         //sabre_get_timing(0);
      }
      if (input == '2') {
         //sabre_all_senarios();
      }
      input = 0;
   }
}


void foilRunAllSenarios() {
   //foilTestHit();
   foilHitA();
   foilHitB();
   foilOffTargetA();
   foilOffTargetB();
   foilShortHitA();
   foilShortHitB();
   foilFAPiste();
   foilFBPiste();
   foilHitAHitB();
   foilHitBHitA();
   foilHitAOffTargetB();
   foilHitBOffTargetA();
   foilOffTargetAHitB();
   foilOffTargetBHitA();
   foilHitALockoutOffTargetB();
   foilHitBLockoutOffTargetA();
   foilHitALockoutHitB();
   foilHitBLockoutHitA();
}


void foilGetLockout(int lockout) {
   Serial.println("finding foil lockout time");
   for (int i = 295; i < 305; i++) {
      Serial.print(i);
      Serial.println("ms");
      digitalWrite(B2_C2, LOW);
      digitalWrite(A1_B2, HIGH);
      delay(i);
      digitalWrite(B1_C1, LOW);
      digitalWrite(A2_B1, HIGH);
      delay(100);
      foilSetup();
      readLights();
      if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
         Serial.println("Double Hit");
      else
         Serial.println("Single Hit");
      delay(3000);
   }
}

void foilGetDepress(int depress) {
   Serial.println("finding foil depress time");
   for (int i = 1; i < 20; i++) {
      Serial.print(i);
      Serial.println("ms");
      digitalWrite(B2_C2, LOW);
      digitalWrite(A1_B2, HIGH);
      delay(i);
      digitalWrite(B2_C2, HIGH);
      digitalWrite(A1_B2, LOW);
      delay(100);
      foilSetup();
      readLights();
      if (onA > 500 && onB < 500 && offA < 500 && offB < 500)
         Serial.println("Hit");
      else
         Serial.println("No Hit");
      delay(3000);
   }
}

// hitA off and
void foilTestHit() {
   Serial.println("A on 10ms released 10ms B on 10ms released");
   digitalWrite(B2_C2, LOW);
   digitalWrite(A1_B2, HIGH);
   delay(10);
   digitalWrite(B2_C2, HIGH);
   digitalWrite(A1_B2, LOW);
   delay(10);
   digitalWrite(B2_C2, LOW);
   digitalWrite(A1_B2, HIGH);
   delay(10);
   digitalWrite(B2_C2, HIGH);
   digitalWrite(A1_B2, LOW);
   foilSetup();
   readLights();
   if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A ontarget for more then the depressed time
void foilHitA() {
   Serial.println("foilHitA");
   digitalWrite(B1_C1, LOW);
   digitalWrite(A2_B1, HIGH);
   delay(50);
   foilSetup();
   readLights();
   if (onA > 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B ontarget for more then the depressed time
void foilHitB() {
   Serial.println("foil hitB");
   digitalWrite(B2_C2, LOW);
   digitalWrite(A1_B2, HIGH);
   delay(50);
   foilSetup();
   readLights();
   if (onA < 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A ontarget for more then the depressed time
void foilOffTargetA() {
   Serial.println("foil_offtargetA");
   digitalWrite(B1_C1, LOW);
   delay(50);
   foilSetup();
   readLights();
   if (onA < 500 && onB < 500 && offA > 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B ontarget for more then the depressed time
void foilOffTargetB() {
   Serial.println("foil offtargetB");
   digitalWrite(B2_C2, LOW);
   delay(50);
   foilSetup();
   readLights();
   if (onA < 500 && onB < 500 && offA < 500 && offB > 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A ontarget for less then the depressed time
void foilShortHitA() {
   Serial.println("foilShortHitA no delay ???");
   digitalWrite(B2_C2, LOW);
   digitalWrite(A1_B2, HIGH);
   foilSetup();
   readLights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B ontarget for less then the depressed time
void foilShortHitB() {
   Serial.println("foilShortHitB no delay ???");
   digitalWrite(B1_C1, LOW);
   digitalWrite(A2_B1, HIGH);
   foilSetup();
   readLights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A hits the piste
void foilFAPiste() {
   /*Serial.println("foilFAPiste");
   digitalWrite(weaponPinA, LOW);
   foilSetup();
   readLights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);*/
}

// fencer B hits the piste
void foilFBPiste() {
   /*Serial.println("foilFBPiste");
   digitalWrite(weaponPinB, LOW);
   foilSetup();
   readLights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);*/
}

// fencer A on target then fencer B on target before lockout
void foilHitAHitB() {
   Serial.println("foilHitAHitB with 40ms delay");
   digitalWrite(B1_C1, LOW);
   digitalWrite(A2_B1, HIGH);
   delay(40);
   digitalWrite(B2_C2, LOW);
   digitalWrite(A1_B2, HIGH);
   delay(100);
   foilSetup();
   readLights();
   if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B on target then fencer A on target before lockout
void foilHitBHitA() {
   Serial.println("foilHitBHitA with 40ms delay");
   digitalWrite(B2_C2, LOW);
   digitalWrite(A1_B2, HIGH);
   delay(40);
   digitalWrite(B1_C1, LOW);
   digitalWrite(A2_B1, HIGH);
   delay(100);
   foilSetup();
   readLights();
   if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A on target then fencer B on target before lockout
void foilHitAOffTargetB() {
   Serial.println("foilHitAOffTargetB with 280ms delay");
   digitalWrite(B1_C1, LOW);
   digitalWrite(A2_B1, HIGH);
   delay(280);
   digitalWrite(B2_C2, LOW);
   delay(100);
   foilSetup();
   readLights();
   if (onA > 500 && onB < 500 && offA < 500 && offB > 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B on target then fencer A on target before lockout
void foilHitBOffTargetA() {
   Serial.println("foilHitBOffTargetA with 280ms delay");
   digitalWrite(B2_C2, LOW);
   digitalWrite(A1_B2, HIGH);
   delay(280);
   digitalWrite(B1_C1, LOW);
   delay(100);
   foilSetup();
   readLights();
   if (onA < 500 && onB > 500 && offA > 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A on target then fencer B on target before lockout
void foilOffTargetAHitB() {
   Serial.println("foilOffTargetAHitB with 280ms delay");
   digitalWrite(B1_C1, LOW);
   delay(280);
   digitalWrite(B2_C2, LOW);
   digitalWrite(A1_B2, HIGH);
   delay(100);
   foilSetup();
   readLights();
   if (onA < 500 && onB > 500 && offA > 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A on target then fencer B on target before lockout
void foilOffTargetBHitA() {
   Serial.println("foilOffTargetBHitA with 280ms delay");
   digitalWrite(B1_C1, LOW);
   digitalWrite(A2_B1, HIGH);
   delay(280);
   digitalWrite(B2_C2, LOW);
   delay(100);
   foilSetup();
   readLights();
   if (onA > 500 && onB < 500 && offA < 500 && offB > 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A on target then fencer B on target before lockout
void foilHitALockoutOffTargetB() {
   Serial.println("foilHitALockoutOffTargetB with 320ms delay");
   digitalWrite(B1_C1, LOW);
   digitalWrite(A2_B1, HIGH);
   delay(320);
   digitalWrite(B2_C2, LOW);
   delay(100);
   foilSetup();
   readLights();
   if (onA > 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B on target then fencer A on target before lockout
void foilHitBLockoutOffTargetA() {
   Serial.println("foilHitBLockoutOffTargetA with 320ms delay");
   digitalWrite(B2_C2, LOW);
   digitalWrite(A1_B2, HIGH);
   delay(320);
   digitalWrite(B1_C1, LOW);
   delay(100);
   foilSetup();
   readLights();
   if (onA < 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer A on target then fencer B on target after lockout
void foilHitALockoutHitB() {
   Serial.println("foilHitALockoutHitB 320ms delay");
   digitalWrite(B1_C1, LOW);
   digitalWrite(A2_B1, HIGH);
   delay(320);
   digitalWrite(B2_C2, LOW);
   digitalWrite(A1_B2, HIGH);
   delay(100);
   foilSetup();
   readLights();
   if (onA > 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}

// fencer B on target then fencer A on target after lockout
void foilHitBLockoutHitA() {
   Serial.println("foilHitBLockoutHitA 320ms delay");
   digitalWrite(B2_C2, LOW);
   digitalWrite(A1_B2, HIGH);
   delay(320);
   digitalWrite(B1_C1, LOW);
   digitalWrite(A2_B1, HIGH);
   delay(100);
   foilSetup();
   readLights();
   if (onA < 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(2000);
}




void epeeRunAllSenarios() {
   epeeSetup();
   epeeHitA();
   epeeHitB();
   epeeShortHitA();
   epeeShortHitB();
   epeeFAPiste();
   epeeFBPiste();
   epeeHitAHitB();
   epeeHitBHitA();
   epeeHitALockoutHitB();
   epeeHitBLockoutHitA();
}

// is a_first_hit true/false
// time_between_hits
// hitB true/false is there a 2nd hit?
void genericEpeeHit(bool a_hit_first, int time_between_hits, bool hitB) {
   /*if (a_hit_first)
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
   readLights();*/
}

void epeeGetLockout(int lockout) {
   Serial.println("finding epee lockout time");
   for (int i = 40; i < 60; i++) {
      Serial.print(i);
      Serial.println("ms");
      digitalWrite(A1_B1, HIGH);
      delay(i);
      digitalWrite(A2_B2, HIGH);
      delay(100);
      epeeSetup();
      readLights();
      if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
         Serial.println("Double Hit");
      else
         Serial.println("Single Hit");
      delay(3000);
   }
}

void epeeGetDepress(int depress) {
   Serial.println("finding epee depress time");
   for (int i = 1; i < 10; i++) {
      Serial.print(i);
      Serial.println("ms");
      digitalWrite(A1_B1, HIGH);
      delay(i);
      digitalWrite(A1_B1, LOW);
      delay(100);
      epeeSetup();
      readLights();
      if (onA > 500 && onB < 500 && offA < 500 && offB < 500)
         Serial.println(" Hit");
      else
         Serial.println("No Hit");
      delay(3000);
   }
}

// fencer A ontarget for more then the depressed time
void epeeHitA() {
   Serial.println("epeeHitA");
   digitalWrite(A1_B1, HIGH);
   delay(50);
   epeeSetup();
   readLights();
   if (onA > 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer B ontarget for more then the depressed time
void epeeHitB() {
   Serial.println("epee hitB");
   digitalWrite(A2_B2, HIGH);
   delay(50);
   epeeSetup();
   readLights();
   if (onA < 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer A ontarget for less then the depressed time
void epeeShortHitA() {
   Serial.println("epeeShortHitA no delay ???");
   digitalWrite(A1_B1, HIGH);
   epeeSetup();
   readLights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer B ontarget for less then the depressed time
void epeeShortHitB() {
   Serial.println("epeeShortHitB no delay ???");
   digitalWrite(A2_B2, HIGH);
   epeeSetup();
   readLights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer A hits the piste
void epeeFAPiste() {
   /*Serial.println("epeeFAPiste");
   digitalWrite(weaponPinA, LOW);
   digitalWrite(lamePinA, LOW);
   delay(100);
   epeeSetup();
   readLights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);*/
}

// fencer B hits the piste
void epeeFBPiste() {
   /*Serial.println("epeeFBPiste");
   digitalWrite(weaponPinB, LOW);
   digitalWrite(lamePinB, LOW);
   delay(100);
   epeeSetup();
   readLights();
   if (onA < 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);*/
}

// fencer A on target then fencer B on target before lockout
void epeeHitAHitB() {
   Serial.println("epeeHitAHitB with 40ms delay");
   digitalWrite(A1_B1, HIGH);
   delay(40);
   digitalWrite(A2_B2, HIGH);
   delay(100);
   epeeSetup();
   readLights();
   if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer B on target then fencer A on target before lockout
void epeeHitBHitA() {
   Serial.println("epeeHitBHitA with 40ms delay");
   digitalWrite(A2_B2, HIGH);
   delay(40);
   digitalWrite(A1_B1, HIGH);
   delay(100);
   epeeSetup();
   readLights();
   if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer A on target then fencer B on target after lockout
void epeeHitALockoutHitB() {
   Serial.println("epeeHitALockoutHitB 50ms delay");
   digitalWrite(A1_B1, HIGH);
   delay(50);
   digitalWrite(A2_B2, HIGH);
   delay(100);
   epeeSetup();
   readLights();
   if (onA > 500 && onB < 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}

// fencer B on target then fencer A on target after lockout
void epeeHitBLockoutHitA() {
   Serial.println("epeeHitBLockoutHitA 50ms delay");
   digitalWrite(A2_B2, HIGH);
   delay(50);
   digitalWrite(A1_B1, HIGH);
   delay(100);
   epeeSetup();
   readLights();
   if (onA < 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}


void readLights() {
   delay(1000);
   offA = analogRead(offTargetALight);
   offB = analogRead(offTargetBLight);
   onA  = analogRead(onTargetALight);
   onB  = analogRead(onTargetBLight);
}
