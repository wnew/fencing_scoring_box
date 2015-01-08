//setup variables
int irPin= 12;
unsigned long lengthHeader;
unsigned long bit;
int byteValue;
int lastTime;


void setup()
{
   pinMode(irPin, INPUT);
   Serial.begin(9600);
}

void loop()
{
   //look for a header pulse from the IR Receiver
   lengthHeader = pulseIn(irPin, LOW);
   if(lengthHeader > 5000 && (millis() - lastTime > 100))
   {
      //step through each of the 32 bits that streams from the remote
      byteValue = 0;
      for(int i = 1; i <= 32; i++)
      {
         bit = pulseIn(irPin, HIGH);

         //read the 8 bits that are specifically the key code
         //use bitwise operations to convert binary to decimal
         if (i > 16 && i <= 24)
            if(bit > 1000)
            byteValue = byteValue + (1 << (i - 17));
       }

      //send the key code to the processing.org program
      Serial.println(byteValue);
      Serial.flush();
   }
   lastTime = millis();
}

