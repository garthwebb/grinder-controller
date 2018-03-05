# grinder-controller

This is an update to the coffee grinder project.  It makes use of the tachometer wires the laser printer motor has to detect when the motor is grinding coffee beans and when its free running.  The existing three position toggle switch (on-momentary, off, on-momentary) used to only make use of one of the on positions to turn on the motor as long as it was pressed.  Now pressing the switch in the other direction will activate an auto grind functionality.  When activated this will grind coffee until it detects there is no more coffee left to grind.

This projet includes the following files and directories:

* Coffee Grinder.pdf - A state diagram describing the new controller
* README.md - This file
* pcb-layout - Eagle files for the PCB designed and created to control the motor.
* src - AVR C code that runs the controller functionality 
