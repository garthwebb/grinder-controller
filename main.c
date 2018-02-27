/**
 *
 */

#include <stdio.h>
#include <avr/io.h>
#include "main.h"
#include <avr/interrupt.h>
#include <stdlib.h>

// define baud
#define BAUD 38400L

// set baud rate value for UBRR
#define BAUD_RATE ((F_CPU/(16L*BAUD))-1L)

#define MOTOR_CTRL PORTC0
#define MOTOR_CTRL_DD DD0
#define MOTOR_CTRL_ON PORTC |= _BV(MOTOR_CTRL)
#define MOTOR_CTRL_OFF PORTC &= ~_BV(MOTOR_CTRL)

#define AUTO_RUN PIND2
#define AUTO_RUN_DD DD2
#define AUTO_RUN_INT PCINT18

#define MANUAL_RUN PIND3
#define MANUAL_RUN_DD DD3
#define MANUAL_RUN_INT PCINT19

#define MOTOR_PULSE PIND4
#define MOTOR_PULSE_DD DD4

// Because we sample every 10ms, this is Frequency/10
#define MIN_FREE_FREQUENCY 188

#define RUN_MODE_FREE 0
#define RUN_MODE_LOADED 1

// Either 1 or 0 depending on whether the current measured input is high or low
uint8_t pulse = 0;
// A counter that tracks high to low transitions
long counter = 0;

uint8_t run_mode = RUN_MODE_FREE;

char msg_buf[80];
long last = 0;

// Flags to raise in the interrupt whenever the manual or auto switch was thrown
uint8_t manual_run_flag = 0;
uint8_t auto_run_flag = 0;
uint16_t debounce_counter = 0;

void init_avr() {
    init_ports();
//    init_counter();
    init_counter_timer();
    init_frequency_timer();
    init_pin_interrupts();

    sei();
}

void init_ports() {
    // Init PORTC

    // Set the motor controller out and initialize it to zero
    DDRC |= _BV(MOTOR_CTRL_DD);
    PORTC &= ~_BV(MOTOR_CTRL);

    // Init PORTD

    // Set some inputs input
    DDRD &= ~_BV(AUTO_RUN_DD) & ~_BV(MANUAL_RUN_DD) & ~_BV(MOTOR_PULSE_DD);

    // Configure all inputs with pull-ups enabled
    PORTD |= _BV(AUTO_RUN_DD) | _BV(MANUAL_RUN_DD) | _BV(MOTOR_PULSE_DD);
}

void init_pin_interrupts() {
    PCMSK2 |= _BV(AUTO_RUN_INT) | _BV(MANUAL_RUN_INT);
    PCICR |= (1<<PCIE2);
}

void init_counter() {
    // External clock source on T0 pin. Clock on rising edge.
    TCCR0B |= (1 << CS02) | (1 << CS01) | (1 << CS00);
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

    // 0.000128*781 = 0.01 s TIMER1_COMPA_vect will be triggered every 10ms
    OCR1AH = 781 >> 8;
    OCR1AL = 781 & 0xFF;
}

ISR(TIMER0_COMPA_vect)
{
    if (PIND & _BV(MOTOR_PULSE)) {
        if (!pulse) {
            pulse = 1;
        }
    } else {
        if (pulse) {
            pulse = 0;
            counter++;
        }
    }
}

ISR(TIMER1_COMPA_vect) 
{
    //last = TCNT0;
    //TCNT0 = 0;
    last = counter;
    counter = 0;
    // sprintf(msg_buf, "%ld\n", last);
    // send(msg_buf);
}

ISR(PCINT2_vect)
{
    if (debounce_counter == 0) {
        // Pins are active low
        if ((PIND & _BV(AUTO_RUN)) == 0) {
            auto_run_flag = 1;
        }
        if ((PIND & _BV(MANUAL_RUN)) == 0) {
            manual_run_flag = 1;
        }
    }
}

/**
 * Initialize UART
 */
void init_uart() {
    // Set baud rate
    UBRR0H = BAUD_RATE >> 8; 
    UBRR0L = BAUD_RATE & 0xFF; 

    // Enable receiver and transmitter
    UCSR0B = _BV(TXEN0) | _BV(RXEN0);

    // 8bit data format
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

void send(const char* string) {
    for (int i = 0; string[i] != 0x00; i++) {
        // Wait until register is free
        while (!(UCSR0A & _BV(UDRE0)));
        UDR0 = (uint8_t) string[i];
    }
}

int main() {
    init_avr();
    init_uart();

    send("-- Initialized\n");

    send("DDRD: ");
    itoa(DDRD, msg_buf, 2);
    send(msg_buf);
    send("\n");

    send("PORTD: ");
    itoa(PORTD, msg_buf, 2);
    send(msg_buf);
    send("\n");

    while (1) {
        if (last >= MIN_FREE_FREQUENCY) {
            if (run_mode == RUN_MODE_LOADED) {
                sprintf(msg_buf, "Free running -- %ld\n", last);
                send(msg_buf);
                run_mode = RUN_MODE_FREE;
            }
        } else {
            if (last != 0 && run_mode == RUN_MODE_FREE) {
                sprintf(msg_buf, "Under load -- %ld\n", last);
                send(msg_buf);
                run_mode = RUN_MODE_LOADED;
            }
        }

        if (manual_run_flag) {
            send("== Detected Manual Request ==\n");
            debounce_counter = 40960;
            manual_run_flag = 0;
        }
        if (auto_run_flag) {
            send("== Detected Auto Request ==\n");
            debounce_counter = 40960;
            auto_run_flag = 0;
        }

        if (debounce_counter > 0) {
            debounce_counter--;
        }
    }
}
