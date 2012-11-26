import serial


device = '/dev/ttyUSB0'
speed = 9600
timeout = 0

ser = serial.Serial(device, speed, timeout=timeout)
print ser.portstr       # check which port was really used

for i in range(0,10000):
   s = ''
   while s == '':
      s = ser.readline()        # read up to ten bytes (timeout)
      if s != '':
      	 print s
      	 if s == 'hitA\r\n':
            print "A"
         if s == "hitB\r\n":
            print "B"
         if s == "offTargetA\r\n":
            print "oA"
         if s == "offTargetB\r\n":
            print "oB"
line = ser.readline()   # read a '\n' terminated line
ser.write("hello")      # write a string
ser.close()