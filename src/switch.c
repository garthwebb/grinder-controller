//
// Created by Garth Webb on 2/27/18.
//

#include <avr/interrupt.h>

#include "switch.h"

uint8_t manual_run_switch_flag = 0;
uint8_t auto_run_switch_flag = 0;

uint16_t debounce_counter = 0;

void init_switch() {
    init_switch_port();
    init_pin_interrupts();
}

void init_switch_port() {
    // Set switch pins as inputs
    DDRD &= ~_BV(AUTO_RUN_SWITCH_DD) & ~_BV(MANUAL_RUN_SWITCH_DD);

    // Configure all inputs with pull-ups enabled
    PORTD |= _BV(AUTO_RUN_SWITCH_DD) | _BV(MANUAL_RUN_SWITCH_DD);
}

void init_pin_interrupts() {
    // Enable switch input pins to trigger an interrupt
    PCMSK2 |= _BV(AUTO_RUN_SWITCH_INT) | _BV(MANUAL_RUN_SWITCH_INT);
    // Set interrupt enable 2 which controls PORTD
    PCICR |= _BV(PCIE2);
}

// Interrupt called when switches are depressed
ISR(PCINT2_vect)
{
    // Ignore the switches if the debounce counter is active unless we've
    if (debounce_counter == 0) {
        // Pins are active low
        if (AUTO_RUN_SWITCH_ACTIVE) {
            auto_run_switch_flag = 1;
            manual_run_switch_flag = 0;
            debounce_counter = DEBOUNCE_DELAY;
        } else if (MANUAL_RUN_SWITCH_ACTIVE) {
            auto_run_switch_flag = 0;
            manual_run_switch_flag = 1;
            debounce_counter = DEBOUNCE_DELAY;
        }
    }
}

void monitor_switches(void) {
    if (debounce_counter > 0) {
        debounce_counter--;
    }
}

// Whether the manual run switch is currently depressed
bool manual_run_switch_on(void) {
    return MANUAL_RUN_SWITCH_ACTIVE;
}

// Ignore input on the manual run switch for a bit
void debounce_manual_run_switch() {
    manual_run_switch_flag = 0;
    debounce_counter = DEBOUNCE_DELAY;
}

// Ignore input on the manual run switch for a bit
void debounce_auto_run_switch() {
    auto_run_switch_flag = 0;
    debounce_counter = DEBOUNCE_DELAY;
}

// Ignore input on both switches
void debounce_switches() {
    manual_run_switch_flag = 0;
    auto_run_switch_flag = 0;
    debounce_counter = DEBOUNCE_DELAY;
}

// Whether the manual run switch was depressed since the last time
// we cleared its switch flag
bool manual_run_switch_tripped(void) {
    if (manual_run_switch_flag) {
        manual_run_switch_flag = 0;
        return true;
    } else {
        return false;
    }
}

// Whether the auto run switch is currently depressed
bool auto_run_switch_on(void) {
    return AUTO_RUN_SWITCH_ACTIVE;
}

// Whether the auto run switch was depressed since the last time
// we cleared its switch flag
bool auto_run_switch_tripped(void) {
    if (auto_run_switch_flag) {
        auto_run_switch_flag = 0;
        return true;
    } else {
        return false;
    }
}

