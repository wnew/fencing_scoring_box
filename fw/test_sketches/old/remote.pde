// 0.1 by pmalmsten http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1176098434
// 0.2 by farkinga
// 0.3 by farkinga - adds cool behaviors

#define IR_BIT_LENGTH 12    // number of bits sent by IR remote
#define BIT_1 1000          // Binary 1 threshold (Microseconds)
#define BIT_0 400           // Binary 0 threshold (Microseconds)
#define BIT_START 2000      // Start bit threshold (Microseconds)

#define IR_PIN 7            // Sensor pin 1 wired through a 220 ohm resistor
#define LED_PIN 9           // first LED output
#define POWER_PIN 11        // second LED output, corresponds to power button

#define DEBUG 1             // Serial connection must be started to debug

int runtime_debug = 0;      // flag to output raw IR pulse data
int output_key = 0;         // flag to print decoded key integers
int power_button = 0;       // flag to indicate if power LED is on
int power_level = 128;      // value (0-255) for power LED intensity

void setup() {
  pinMode(LED_PIN, OUTPUT);	//This shows when we're ready to recieve
  pinMode(POWER_PIN, OUTPUT);	//This is the "power on" indicator
  pinMode(IR_PIN, INPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.begin(9600);
  Serial.println("Start");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  int key = get_ir_key();

  digitalWrite(LED_PIN, LOW);  // turn LED off while processing response
  Serial.println("Key ");
  Serial.println(key);
  delay(100);                  // short delay to cancel duplicate keypresses
}

/*
  wait for a keypress from the IR remote, and return the
  integer mapping of that key (e.g. power button on remote returns
  the integer 1429)
*/

int get_ir_key()
{
  int pulse[IR_BIT_LENGTH];
  int bits[IR_BIT_LENGTH];

  do {} //Wait for a start bit
  while(pulseIn(IR_PIN, LOW) < BIT_START);

  read_pulse(pulse, IR_BIT_LENGTH);
  pulse_to_bits(pulse, bits, IR_BIT_LENGTH);
  return bits_to_int(bits, IR_BIT_LENGTH);
}

/*
  use pulseIn to receive IR pulses from the remote.
  Record the length of these pulses (in ms) in an array
*/

void read_pulse(int pulse[], int num_bits)
{
  for (int i = 0; i < num_bits; i++)
  {
    pulse[i] = pulseIn(IR_PIN, LOW);
  }
}

/*
  IR pulses encode binary "0" as a short pulse, and binary "1"
  as a long pulse.  Given an array containing pulse lengths,
  convert this to an array containing binary values
*/

void pulse_to_bits(int pulse[], int bits[], int num_bits)
{
  if (DEBUG || runtime_debug) { Serial.println("-----"); }

  for(int i = 0; i < num_bits ; i++)
  {
    if (DEBUG || runtime_debug) { Serial.println(pulse[i]); }

    if(pulse[i] > BIT_1) //is it a 1?
    {
      bits[i] = 1;
    }
    else if(pulse[i] > BIT_0) //is it a 0?
    {
      bits[i] = 0;
    }
    else //data is invalid...
    {
      Serial.println("Error");
    }
  }
}

/*
  convert an array of binary values to a single base-10 integer
*/

int bits_to_int(int bits[], int num_bits)
{
  int result = 0;
  int seed = 1;

  //Convert bits to integer
  for(int i = 0 ; i < num_bits ; i++)
  {
    if(bits[i] == 1)
    {
	result += seed;
    }

    seed *= 2;
  }

  return result;
}

/*
  make LED blink rapidly
*/

void blink_led()
{
  for (int i = 0; i < 5; i++)
  {
    analogWrite(LED_PIN, 0);
    delay(50);
    analogWrite(LED_PIN, 1);
    delay(50);
  }
}

/*
  neat little routine to fade both LEDs in a sequence.  Currently,
  this is called by do_response() when the "play" button is pressed.
*/

