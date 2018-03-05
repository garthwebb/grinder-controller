//
// Created by Garth Webb on 2/27/18.
//

#ifndef GRINDER_CONTROLLER_SWITCH_H
#define GRINDER_CONTROLLER_SWITCH_H

#include <stdint.h>
#include <stdbool.h>

#define AUTO_RUN_SWITCH PIND2
#define AUTO_RUN_SWITCH_DD DD2
#define AUTO_RUN_SWITCH_INT PCINT18

#define MANUAL_RUN_SWITCH PIND3
#define MANUAL_RUN_SWITCH_DD DD3
#define MANUAL_RUN_SWITCH_INT PCINT19

#define AUTO_RUN_SWITCH_ACTIVE ((PIND & _BV(AUTO_RUN_SWITCH)) == 0)
#define MANUAL_RUN_SWITCH_ACTIVE ((PIND & _BV(MANUAL_RUN_SWITCH)) == 0)

// Number of event loop cycles to wait before accepting new input from the switches
#define DEBOUNCE_DELAY 40960

void init_switch(void);

void init_switch_port(void);

void init_pin_interrupts(void);

// Take care of any switch state in the main event loop
void monitor_switches(void);

// Whether the manual run switch is currently depressed
bool manual_run_switch_on(void);

// Explicitly clear the manual run switch flag
void debounce_manual_run_switch(void);

// Explicitly clear the auto run switch flag
void debounce_auto_run_switch(void);

// Explicitly clear both switches
void debounce_switches(void);

// Whether the manual run switch was depressed since the last time
// we read and cleared its switch flag
bool manual_run_switch_tripped(void);

// Whether the auto run switch is currently depressed
bool auto_run_switch_on(void);

// Whether the auto run switch was depressed since the last time
// we read and cleared its switch flag
bool auto_run_switch_tripped(void);

#endif //GRINDER_CONTROLLER_SWITCH_H
