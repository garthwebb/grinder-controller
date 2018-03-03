//
// Created by Garth Webb on 2/27/18.
//

#include <stdint.h>
#include "state_machine.h"
#include "motor.h"
#include "timers.h"
#include "switch.h"
#include "serial.h"

#define STATE_STOPPED 0
#define STATE_AUTO_RUN_STARTING 1
#define STATE_AUTO_RUN 2
#define STATE_AUTO_RUN_FINISHING 3
#define STATE_MANUAL_RUN_STARTING 4
#define STATE_MANUAL_RUN 5
#define STATE_MANUAL_RUN_CLEARING 6

uint8_t current_state = STATE_STOPPED;
bool message_printed = false;

void evaluate_state() {
    switch(current_state) {
        case STATE_AUTO_RUN_STARTING:
            handle_auto_run_starting();
            break;
        case STATE_AUTO_RUN:
            handle_auto_run();
            break;
        case STATE_AUTO_RUN_FINISHING:
            handle_auto_run_finishing();
            break;
        case STATE_MANUAL_RUN_STARTING:
            handle_manual_run_starting();
            break;
        case STATE_MANUAL_RUN:
            handle_manual_run();
            break;
        case STATE_MANUAL_RUN_CLEARING:
            handle_manual_run_clearing();
            break;
        case STATE_STOPPED:
        default:
            handle_stopped();
    }
}

// While the grinder starts up, it will be at low speeds for a few
// cycles.  Allow this without shutting down due to being below
// the speed threshold
void handle_auto_run_starting() {
    if (!message_printed) {
        send("== STATE: auto_run_starting\n");
        message_printed = true;
    }

    // Detect steady state
    if (motor_frequency() >= MIN_SAFE_FREQUENCY) {
        send_int("** Frequency above min safe threshold: %d\n", motor_frequency());
        transition_auto_run();
        return;
    }

    // Detect too long below threshold
    if (elapsed_timer_greater_than(STARTING_TIME)) {
        send_int("** Timer ran out, frequency below threshold: %d\n", motor_frequency());
        transition_stopped();
    }
}

void handle_auto_run() {
    if (!message_printed) {
        send("== STATE: auto_run\n");
        message_printed = true;
    }

    // Detect speed increase
    if (motor_frequency() >= MIN_FREE_FREQUENCY) {
        send_int("** Safe frequency reached above threshold: %d\n", motor_frequency());
        transition_auto_run_finishing();
        return;
    }

    // Detect speed below threshold
    if (motor_frequency() < MIN_SAFE_FREQUENCY) {
        send_int("** Frequency below safe threshold: %d\n", motor_frequency());
        transition_stopped();
        return;
    }

    // Detect manual switch depress
    if (manual_run_switch_tripped()) {
        send("** Manual run switch tripped\n");
        transition_manual_run();
    }
}

void handle_auto_run_finishing() {
    if (!message_printed) {
        send("== STATE: auto_run_finishing\n");
        message_printed = true;
    }

    // Detect wait time elapsed
    if (elapsed_timer_greater_than(AUTO_FINISH_TIME)) {
        send("** Auto finish time elapsed\n");
        transition_stopped();
        return;
    }

    // Detect speed decrease
    if (motor_frequency() < MIN_FREE_FREQUENCY) {
        send_int("** Speed decreased to loaded frequency: %d\n", motor_frequency());
        transition_auto_run();
        return;
    }

    // Detect manual switch depress
    if (manual_run_switch_tripped()) {
        send("** Manual run switch tripped\n");
        transition_manual_run();
    }
}

void handle_manual_run_starting() {
    if (!message_printed) {
        send("== STATE: manual_run_starting\n");
        message_printed = true;
    }

    // Detect steady state
    if (motor_frequency() >= MIN_SAFE_FREQUENCY) {
        send_int("** Safe frequency reached above threshold: %d\n", motor_frequency());
        transition_manual_run();
        return;
    }

    // Detect too long below threshold
    if (elapsed_timer_greater_than(STARTING_TIME)) {
        send_int("** Timer ran out, frequency below threshold: %d\n", motor_frequency());
        transition_stopped();
    }

    // Detect switch released
    if (!manual_run_switch_on()) {
        send("** Manual run switch released\n");
        transition_stopped();
    }
}

void handle_manual_run() {
    if (!message_printed) {
        send("== STATE: manual_run\n");
        message_printed = true;
    }

    // Detect manual switch not depressed
    if (!manual_run_switch_on()) {
        send("** Manual run switch released\n");
        transition_stopped();
        return;
    }

    // Detect speed below threshold
    if (motor_frequency() < MIN_SAFE_FREQUENCY) {
        send_int("** Frequency below safe threshold: %d\n", motor_frequency());
        transition_manual_run_clearing();
    }
}

void handle_manual_run_clearing() {
    if (!message_printed) {
        send("== STATE: manual_run_clearing\n");
        message_printed = true;
    }

    // Hold here in this (error) state with the motor stopped until the user releases the manual run button.
    // This makes sure we don't drop to the stopped state and start up again because the switch
    // is still depressed.
    if (!manual_run_switch_on()) {
        send("** Manual run switch released\n");
        transition_stopped();
    }
}

void handle_stopped() {
    if (!message_printed) {
        send("== STATE: stopped\n");
        message_printed = true;
    }

    // Detect manual switch depressed.  We want to know that its pressed
    // continually, so just check that its on.
    if (manual_run_switch_on()) {
        send("** Manual switch depressed\n");
        transition_manual_run_starting();
    }

    // Detect auto run switch depressed.  We want to catch that it was
    // pressed previously even if it didn't happen this instant.
    if (auto_run_switch_tripped()) {
        send("** Auto run switch triggered\n");
        transition_auto_run_starting();
    }
}

void transition_auto_run_starting() {
    send("--> Transition to auto_run_starting\n\n");
    start_motor();
    clear_elapsed_timer();
    current_state = STATE_AUTO_RUN_STARTING;
    message_printed = false;
}

void transition_auto_run() {
    send("--> Transition to auto_run\n\n");
    current_state = STATE_AUTO_RUN;
    message_printed = false;
}

void transition_auto_run_finishing() {
    send("--> Transition to auto_run_finishing\n\n");
    clear_elapsed_timer();
    current_state = STATE_AUTO_RUN_FINISHING;
    message_printed = false;
}

void transition_manual_run_starting() {
    send("--> Transition to manual_run_starting\n\n");
    start_motor();
    clear_elapsed_timer();
    current_state = STATE_MANUAL_RUN_STARTING;
    message_printed = false;
}

void transition_manual_run() {
    send("--> Transition to manual_run\n\n");
    current_state = STATE_MANUAL_RUN;
    message_printed = false;
}

void transition_manual_run_clearing() {
    send("--> Transition to manual_run_clearing\n\n");
    stop_motor();
    current_state = STATE_MANUAL_RUN_CLEARING;
    message_printed = false;
}

void transition_stopped() {
    send("--> Transition to stopped\n\n");
    stop_motor();
    current_state = STATE_STOPPED;

    // Handle bounces switches before moving to stopped
    debounce_switches();

    message_printed = false;
}
