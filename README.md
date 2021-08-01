# grinder-controller

A motor controller for the coffee grinder project.  It makes use of the tachometer wires from the motor to detect when the motor is grinding coffee beans vs when its free running.  A three position toggle switch (on-momentary, off, on-momentary) is used to control the grinder; toggling and releasing in one direction will start automatic grinding, where the motor will only turn off when it detects no more coffee is present.  Toggling in the other direction will grind until the toggle is released, then stop.

This projet includes the following files and directories:

* Coffee Grinder.pdf - A state diagram describing the new controller
* README.md - This file
* pcb-layout - Eagle files for the PCB designed and created to control the motor.
* src - AVR C code that runs the controller functionality 
