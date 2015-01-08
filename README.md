 Description
-------------------------------------------------------------------------------
Fencing Scoring Machine/Box/Apparatus code for the arduino platform.
This project is an arduino based fencing scoring machine. It supports all 3
weaspons, foil, epee and sabre. There are also individual sketched for each
weapon if you would like only one weapon. The 2 fencers connect to the arduino
and the ground connected to the piste.

Email me if you have any questions: wesnew@gmail.com

 Directory Structure
-------------------------------------------------------------------------------
Software: python code which communicates over the serial interface with the
arduino to allow displaying of lights on a laptop or PC screen

Firmware, the code that runs on the arduino including scoring boxes and test
sketches

Hardware, the circuit designs around the arduino showing how to connect up the
lights and body wires



 Hardware Requirements
-------------------------------------------------------------------------------
 - 1 Arduino Pro Micro/Pro Mini/Uno/Mega/Due
 - 10 Resistors, 4 220R for LEDs, 6 10K for pullups/pulldowns
 - 4 LEDs, 1 green, 1 red and 2 yellow/white


 Percentage Complete
-------------------------------------------------------------------------------

        85 %



 TODOs
-------------------------------------------------------------------------------
 - Add support for the ground lights
 - Possibly use shit reg for lights
 - Add interrupts for buttons


 Testing
-------------------------------------------------------------------------------
 - Connect up to a couple of fencers and test
 - Create a test arduino sketch to check that the timings (Lockout and depress)
   are correct


 Extra features to add
-------------------------------------------------------------------------------
 - Add some kind of screen
 - USB interface to setup a laptop as the lights and scoring
 - Bluetooth for configuration
 - Connect to a network using the Ethernet shield (Not planning to do anytime soon)
