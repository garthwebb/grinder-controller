//
// Created by Garth Webb on 2/27/18.
//

#ifndef GRINDER_CONTROLLER_SERIAL_H
#define GRINDER_CONTROLLER_SERIAL_H

// define baud
#define BAUD 38400L

// set baud rate value for UBRR
#define BAUD_RATE ((F_CPU/(16L*BAUD))-1L)

void init_serial(void);
void send(const char *string);
void send_int(const char* string, uint8_t val);

#endif //GRINDER_CONTROLLER_SERIAL_H
