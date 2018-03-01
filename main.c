/**
 * Main program for coffee grinder controller
 *
 * The coffee grinder has a three position momentary toggle switch.
 *
 * - Resting position (middle) : No action
 * - Up : Start auto grind mode
 * - Down : Manual grind mode for as long as depressed
 */

#include <avr/interrupt.h>

#include "main.h"
#include "motor.h"
#include "serial.h"
#include "state_machine.h"
#include "switch.h"
#include "timers.h"

void init_avr() {
    init_serial();
    init_timers();
    init_switch();
    init_motor();
    init_pin_interrupts();

    send("-- Initialized\n");

    // Enable interrupts
    sei();
}

int main() {
    init_avr();

    while (1) {
        monitor_switches();
        evaluate_state();
    }
}
