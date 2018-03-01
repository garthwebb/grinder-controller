//
// Created by Garth Webb on 2/27/18.
//

#include <avr/io.h>
#include <stdio.h>
#include "serial.h"

char buf[160];

/**
 * Initialize UART
 */
void init_serial() {
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

void send_int(const char* string, uint8_t val) {
    sprintf(buf, string, val);
    send(buf);
}