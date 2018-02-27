#ifndef _MAIN_H
#define _MAIN_H

void init_avr(void);
void init_ports(void);
void init_counter(void);
void init_pin_interrupts(void);
void init_counter_timer(void);
void init_frequency_timer(void);
void init_uart(void);

void send(const char *string);

int main(void);

#endif
