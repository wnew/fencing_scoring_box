//===========================================================================//
//                                                                           //
//  Desc: Arduino Code to implement a foil scoring apparatus                 //
//  Dev:  DigitalWestie & Wnew                                               //
//  Date: Nov 2012                                                           //
//  Notes: Origonal code from digitalwestie on github                        //
//         Plan to edit to include other weapons                             //
//                                                                           //
//===========================================================================//

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

long millisPastA     = 0;
long millisPastB     = 0;
long millisPastFirst = 0;

char input = ' ';

int voltageThresh = 500;         // the threshold that the scoring triggers on

void setup() {
   pinMode(offTargetA, INPUT);
   pinMode(offTargetB, INPUT);
   pinMode(onTargetA,  INPUT);
   pinMode(onTargetB,  INPUT);
   
   pinMode(weaponPinA, OUTPUT);     
   pinMode(weaponPinB, OUTPUT);     
   pinMode(lamePinA,   OUTPUT);    
   pinMode(lamePinB,   OUTPUT);
   
   Serial.begin(9600);   
   resetValues();
}

void loop()
{
   menu();

}

void menu()
{
   Serial.println("Select Weapon:");
   Serial.println("1. Foil");
   Serial.println("2. Epee");
   Serial.println("3. Sabre");
   while(!Serial.available());
   input = Serial.read();
   if (input == '1')
   {
      // foil menu
   }
   if (input == '2')
   {
      // epee menu
      Serial.println("Select Test:");
      Serial.println("1. Get Lockout Time");
      Serial.println("2. Run all senarios test");
      while(!Serial.available());
      input = Serial.read();
      if (input == '1')
      {
         epee_get_timing(0);
      }
      if (input == '2')
      {
         epee_all_senarios();
      }
   }
   if (input == '3')
   {
      // sabre menu
   }
}


void epee_all_senarios()
{
   epee_hitA();  
   resetValues();
   epee_hitB();
   resetValues();
   epee_short_hitA();  
   resetValues();
   epee_short_hitB();  
   resetValues();
   epee_fA_piste();
   resetValues();
   epee_fB_piste();
   resetValues();
   epee_hitA_hitB();
   resetValues();
   epee_hitB_hitA();
   resetValues();
   epee_hitA_lockout_hitB();
   resetValues();
   epee_hitB_lockout_hitA();
   resetValues();
}

void epee_get_timing(int lockout)
{
   Serial.println("finding epee lockout time");
   for (int i = 40; i < 60; i++)
   {
      Serial.print(i);
      Serial.println("ms");
      digitalWrite(weaponPinB, LOW);
      delay(i);
      digitalWrite(weaponPinA, LOW);
      delay(100);
      digitalWrite(weaponPinB, HIGH);
      digitalWrite(weaponPinA, HIGH);
      delay(1000);
      offA = analogRead(offTargetA);
      offB = analogRead(offTargetB);
      onA  = analogRead(onTargetA);
      onB  = analogRead(onTargetB);
      if (onA > 500 && onB > 500 && offA < 500 && offB < 500)
         Serial.println("Double Hit");
      else
         Serial.println("Single Hit");
      delay(3000);
   }
}

// fencer A ontarget for more then the depressed time
void epee_hitA()
{
   Serial.println("epee_hitA");
   digitalWrite(weaponPinA, LOW);
   delay(50);
   digitalWrite(weaponPinA, HIGH);
   delay(1000);
   offA = analogRead(offTargetA);
   offB = analogRead(offTargetB);
   onA  = analogRead(onTargetA);
   onB  = analogRead(onTargetB);
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
   digitalWrite(weaponPinB, LOW);
   delay(50);
   digitalWrite(weaponPinB, HIGH);
   delay(1000);
   offA = analogRead(offTargetA);
   offB = analogRead(offTargetB);
   onA  = analogRead(onTargetA);
   onB  = analogRead(onTargetB);
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
   digitalWrite(weaponPinA, LOW);
   digitalWrite(weaponPinA, HIGH);
   delay(1000);
   offA = analogRead(offTargetA);
   offB = analogRead(offTargetB);
   onA  = analogRead(onTargetA);
   onB  = analogRead(onTargetB);
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
   digitalWrite(weaponPinB, LOW);
   digitalWrite(weaponPinB, HIGH);
   delay(1000);
   offA = analogRead(offTargetA);
   offB = analogRead(offTargetB);
   onA  = analogRead(onTargetA);
   onB  = analogRead(onTargetB);
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
   digitalWrite(weaponPinA, LOW);
   digitalWrite(lamePinA, LOW);
   delay(1000);
   offA = analogRead(offTargetA);
   offB = analogRead(offTargetB);
   onA  = analogRead(onTargetA);
   onB  = analogRead(onTargetB);
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
   digitalWrite(weaponPinB, LOW);
   digitalWrite(lamePinB, LOW);
   delay(1000);
   offA = analogRead(offTargetA);
   offB = analogRead(offTargetB);
   onA  = analogRead(onTargetA);
   onB  = analogRead(onTargetB);
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
   digitalWrite(weaponPinA, LOW);
   delay(40);
   digitalWrite(weaponPinB, LOW);
   delay(100);
   digitalWrite(weaponPinA, HIGH);
   digitalWrite(weaponPinB, HIGH);
   delay(1000);
   offA = analogRead(offTargetA);
   offB = analogRead(offTargetB);
   onA  = analogRead(onTargetA);
   onB  = analogRead(onTargetB);
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
   digitalWrite(weaponPinB, LOW);
   delay(40);
   digitalWrite(weaponPinA, LOW);
   delay(100);
   digitalWrite(weaponPinB, HIGH);
   digitalWrite(weaponPinA, HIGH);
   delay(1000);
   offA = analogRead(offTargetA);
   offB = analogRead(offTargetB);
   onA  = analogRead(onTargetA);
   onB  = analogRead(onTargetB);
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
   digitalWrite(weaponPinA, LOW);
   delay(50);
   digitalWrite(weaponPinB, LOW);
   delay(100);
   digitalWrite(weaponPinA, HIGH);
   digitalWrite(weaponPinB, HIGH);
   delay(1000);
   offA = analogRead(offTargetA);
   offB = analogRead(offTargetB);
   onA  = analogRead(onTargetA);
   onB  = analogRead(onTargetB);
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
   digitalWrite(weaponPinB, LOW);
   delay(50);
   digitalWrite(weaponPinA, LOW);
   delay(100);
   digitalWrite(weaponPinB, HIGH);
   digitalWrite(weaponPinA, HIGH);
   delay(1000);
   offA = analogRead(offTargetA);
   offB = analogRead(offTargetB);
   onA  = analogRead(onTargetA);
   onB  = analogRead(onTargetB);
   if (onA < 500 && onB > 500 && offA < 500 && offB < 500)
      Serial.println("Pass");
   else
      Serial.println("Fail");
   delay(4000);
}


void resetValues()
{
   digitalWrite(weaponPinA, HIGH);
   digitalWrite(weaponPinB, HIGH);
   digitalWrite(lamePinA, HIGH);
   digitalWrite(lamePinB, HIGH);
     
   millisPastA = millis();
   millisPastB = millis();
   millisPastFirst = 0;

   delay(100);
}
