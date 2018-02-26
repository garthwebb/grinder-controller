/**
 *
 */

#include <stdio.h>
#include <avr/io.h>
#include "main.h"
#include <avr/interrupt.h>

// define baud
#define BAUD 38400L

// set baud rate value for UBRR
#define BAUD_RATE ((F_CPU/(16L*BAUD))-1L)

#define MOTOR_CTRL PORTC0
#define MOTOR_CTRL_DD DD0
#define MOTOR_CTRL_ON PORTC |= _BV(MOTOR_CTRL)
#define MOTOR_CTRL_OFF PORTC &= ~_BV(MOTOR_CTRL)

#define AUTO_RUN PORTD2
#define AUTO_RUN_DD DD2

#define MANUAL_RUN PORTD3
#define MANUAL_RUN_DD DD3

#define MOTOR_PULSE PORTD4
#define MOTOR_PULSE_DD DD4

char msg_buf[80];
int last = 0;

void init_avr() {
    init_ports();
    init_counter();
    init_timer();

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

void init_counter() {
    // External clock source on T0 pin. Clock on rising edge.
    TCCR0B |= (1 << CS02) | (1 << CS01) | (1 << CS00);
}

void init_timer() {
    // Enable Interrupt Timer/Counter1, Output Compare A (TIMER1_COMPA_vect)
    TIMSK1 = _BV(OCIE1A);

    // Clock/1024, 0.000128 seconds per tick, Mode=CTC
    TCCR1B = _BV(CS12) | _BV(CS10) | _BV(WGM12);

    // 0.000128*781 = 0.01 s TIMER1_COMPA_vect will be triggered every 10ms
    OCR1AH = 781 >> 8;
    OCR1AL = 781 & 0xFF;
}

ISR(TIMER1_COMPA_vect) 
{
    last = TCNT0;
    TCNT0 = 0;
    sprintf(msg_buf, "%d\n", last);
    send(msg_buf);
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

    while (1) {
    }
}
