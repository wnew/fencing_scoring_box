#!/usr/bin/python
import Tkinter
import tkMessageBox
import serial
import time
import threading
import Image
import ImageTk

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
      
      # open a SPIDER image and convert to byte format
      im = Image.open('red.png')
      # Convert the Image object into a TkPhoto object
      tkimage = ImageTk.PhotoImage(im)
      # Put it in the display window
      Tkinter.Label(self.top, image=tkimage).pack()

      display.pack()
      self.top.mainloop()




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

app = MyTkApp()
