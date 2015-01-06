 Description
-------------
Fencing Scoring Machine/Box/Apparatus code for the arduino platform.
This project is an arduino based fencing scoring machine. It supports all 3
weaspons, foil, epee and sabre. The 2 fencers connect to the arduino and the
ground connected to the piste.

email me if you have any questions: wesnew@gmail.com

 Directory Structure
-------------
sw - Software, python code which communicates over the serial interface with the arduino

fw - Firmware, the code that runs on the arduino

hw - Hardware, the circuit designs around the arduino



 Hardware Requirements
-------------
1 Arduino Pro Micro/Pro Mini/Uno/Mega
6 resistors


 Percentage Complete
-------------
        
        85 %



 TODOs
-------------
 - Add support for the ground lights
 - Possibly use shit reg for lights
 - Add interrupts for buttons


 Testing
-------------
 - Connect up to a couple of fencers and test
 - Create a test arduino sketch to check that the timings (Lockout and depress)
   are correct

 Extra features to add
-------------
 - Add some kind of screen
 - Bluetooth for configuration
 - Connect to a network using the Ethernet shield (Not planning to do anytime soon)
 - USB interface to setup a laptop as the lights and scoring
