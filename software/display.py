#!/usr/bin/python
import Tkinter
import tkMessageBox
import serial
import time
import threading

class MyTkApp(threading.Thread):
   def __init__(self):
      threading.Thread.__init__(self)
      self.start()
   def callback(self):
      self.root.quit()
   def run(self):
      self.top = Tkinter.Tk()
      global display
      display = Tkinter.Canvas(self.top, height=250, width=950)
      display.pack()
      self.top.mainloop()
      #self.root=Tkinter.Tk()
      #self.root.protocol("WM_DELETE_WINDOW", self.callback)
      #self.s = Tkinter.StringVar()
      #self.s.set('Foo')#l = Tkinter.Label(self.root,textvariable=self.s)
      #l.pack()
      #self.root.mainloop()

class Timer:

   def StartTimer(self):
      self.TimerOffset = time.time()
      self.LastTicked = 0
      self.TimeWhenItWasPaused = 0
      self.started = False
      self.paused = False
   
   def Tick(self):
      if self.paused is False:
         NewTicked = time.time() - self.TimerOffset
         diff = NewTicked - self.LastTicked
         self.LastTicked = NewTicked
         return diff
      else:
         print "Cannot Tick, Timer is paused"

   def GetTime(self):
      if self.started is False:
         return 0
      if self.paused is True:
         return self.TimeWhenItWasPaused
      else:
         return time.time() - self.TimerOffset
      
   def Pause(self):
      self.TimeWhenItWasPaused = time.time() - self.TimerOffset
      self.paused = True

   def Unpause(self):
      self.TimerOffset = time.time() - self.TimeWhenItWasPaused
      self.paused = False


def reset_lights(self):
   global coord1
   coord1 =  10, 10, 200, 200
   global coord2
   coord2 = 210, 10, 400, 200
   global coord3
   coord3 = 410, 10, 600, 200
   global coord4
   coord4 = 610, 10, 800, 200
   self.on_targ_red  = display.create_rectangle(coord1, fill="grey")
   self.off_targ_red = display.create_rectangle(coord2, fill="grey")
   self.off_targ_grn = display.create_rectangle(coord3, fill="grey")
   self.on_targ_grn  = display.create_rectangle(coord4, fill="grey")


#reset_lights()
class serial_coms():
   def __init__(self, device, speed, timeout):
      self.ser = serial.Serial(device, speed, timeout=timeout)
      # create new timer here
      print self.ser.portstr       # check which port was really used
   def wait_for_command(self):
      reset_lights(self)
      s = ""
      while 1:
         s = self.ser.readline()        # read (timeout)
         if s != '':
            #print s
            if s == 'A':
               self.on_targ_red  = display.create_rectangle(coord1, fill="red")
               # pause timer here
            if s == "C":
               self.off_targ_red = display.create_rectangle(coord2, fill="yellow")
               # pause timer here
            if s == "D":
               self.off_targ_grn = display.create_rectangle(coord3, fill="yellow")
               # pause timer here
            if s == "B":
               self.on_targ_grn  = display.create_rectangle(coord4, fill="green")
               # pause timer here
            if s == "R":
               reset_lights(self)
      self.ser.close()

app = MyTkApp()
serial_coms = serial_coms('/dev/ttyUSB0', 9600, 0)
serial_coms.wait_for_command()
