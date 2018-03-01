//
// Created by Garth Webb on 2/27/18.
//

#include <avr/io.h>
#include "motor.h"

void init_motor() {
    // Set the motor controller out and initialize it to zero
    DDRC |= _BV(MOTOR_CTRL_DD);
    PORTC &= ~_BV(MOTOR_CTRL);
}

void start_motor() {
    MOTOR_CTRL_ON;
}

void stop_motor() {
    MOTOR_CTRL_OFF;
}