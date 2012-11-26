#include <avr/interrupt.h>
#include <avr/io.h> 
#define INIT_TIMER_COUNT 6 
#define RESET_TIMER2 TCNT2 = INIT_TIMER_COUNT 
int ledPin = 13; 
int int_counter = 0; 
volatile int second = 0;
int oldSecond = 0;
long starttime = 0;

// Aruino runs at 16 Mhz, so we have 1000 Overflows per second... 
// 1/ ((16000000 / 64) / 256) = 1 / 1000 

ISR(TIMER2_OVF_vect) {
  RESET_TIMER2; int_counter += 1;
  if (int_counter == 1000) {
    second+=1; int_counter = 0;
  }
};
  
void setup() {
  Serial.begin(9600);
  Serial.println("Initializing timerinterrupt");
  //Timer2 Settings: Timer Prescaler /64, 
  TCCR2 |= (1< <<<"");
  Serial.print(millis() - starttime); Serial.println(".");
  digitalWrite(ledPin, HIGH); delay(100);
  digitalWrite(ledPin, LOW); oldSecond = second;
}
}
