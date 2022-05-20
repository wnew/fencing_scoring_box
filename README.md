# Arduino Fencing Scoring Apparatus


## Description

Fencing Scoring Machine/Box/Apparatus code for the arduino platform.
This project is an arduino based fencing scoring machine. It supports all 3
weaspons, foil, epee and sabre. There are also individual sketched for each
weapon if you would like only one weapon.

Detailed assembly instructions are on instructables: 
https://www.instructables.com/id/Arduino-Fencing-Scoring-Apparatus/

Email me if you have any questions: wesnew(at)gmail(dot)com


## Directory Structure

Design: Notes and research. 

**Software**: python code which communicates over the serial interface with the
arduino to allow displaying of lights on a laptop or PC screen.

**Firmware**: the code that runs on the arduino including scoring boxes and test
sketches.

**Hardware**: the circuit designs around the arduino showing how to connect up the
lights and body wires.


## Hardware Requirements

 - 1 Arduino Uno/Mega or Pro Mini (5v/16MHz)
 - 10 Resistors, 4 220R for LEDs, 6 1K for pullups/pulldowns
 - 4 LEDs, 1 green, 1 red and 2 yellow/white


## Percentage Complete

Foil        90 %

Epee       100 %

Sabre       50 %


## TODOs

 - Add support for the ground lights
 - Test sabre (quite a bit of work here)
 - Implement whipover for sabre


## Testing

 - Tested with an oscilloscope and all timings look great.
 - Now to connect it up to a couple of real life fencers and make them duel
 - Create a test arduino sketch to check that the timings (Lockout and depress)
   are correct


## Extra features to add

 - Add some kind of screen
 - Serial to USB interface to use a laptop as the lights, timing and scoring,
   similar to the VSM. This is already implemented on the device, just write
   some code for the PC side. 
