//
// Created by Garth Webb on 2/27/18.
//

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timers.h"

// General timer to record elapsed milliseconds
uint16_t elapsed_timer = 0;

// Keeps a count of pulses over the previous time period
uint8_t frequency_counter = 0;

// Holds the last frequency recorded
uint8_t current_frequency = 0;

// Whether the current waveform is high (1) or low (0)
uint8_t waveform = 0;

void init_timers() {
    init_counter_port();
    init_counter_timer();
    init_frequency_timer();
}

void init_counter_port() {
    // Set motor pulse pin as an input
    DDRD &= ~_BV(MOTOR_PULSE_DD);

    // Configure input with pull-up enabled
    PORTD |= _BV(MOTOR_PULSE_DD);
}

void init_counter_timer() {
    // Enable Interrupt TimerCounter0 Compare Match A (TIMER0_COMPA_vect)
    TIMSK0 = _BV(OCIE0A);
    // Mode = CTC
    TCCR0A = _BV(WGM01);
    // 1/(Clock/Scaling) == 1/(8_000_000/64) == 0.000008 s == 8 us per tick
    TCCR0B = _BV(CS01) | _BV(CS00);
    // 0.000008*5 == 0.00004 == 40us TIMER0_COMPA_vect will be triggered 25000 times per second.
    OCR0A = 5;
}

void init_frequency_timer() {
    // Enable Interrupt Timer/Counter1, Output Compare A (TIMER1_COMPA_vect)
    TIMSK1 = _BV(OCIE1A);

    // 1/(Clock/Scaling) == 1/(8_000_000/1024) == 0.000128 seconds per tick, Mode=CTC
    TCCR1B = _BV(CS12) | _BV(CS10) | _BV(WGM12);

    // 0.000128*781 ~= 0.1 s TIMER1_COMPA_vect will be triggered every 1ms
    OCR1AH = 781 >> 8;
    OCR1AL = 781 & 0xFF;
}

// Runs every 8 microseconds
ISR(TIMER0_COMPA_vect)
{
    if (PIND & _BV(MOTOR_PULSE)) {
        if (!waveform) {
            waveform = 1;
        }
    } else {
        if (waveform) {
            waveform = 0;
            frequency_counter++;
        }
    }
}

// Runs every 10 milliseconds
ISR(TIMER1_COMPA_vect)
{
    current_frequency = frequency_counter;
    frequency_counter = 0;
    elapsed_timer++;
}

uint8_t motor_frequency() {
    return current_frequency;
}

void clear_elapsed_timer() {
    elapsed_timer = 0;
}

bool elapsed_timer_greater_than(uint16_t limit) {
    return elapsed_timer > limit;
}