//
// Created by Garth Webb on 2/27/18.
//

#include <stdbool.h>

#ifndef GRINDER_CONTROLLER_TIMERS_H
#define GRINDER_CONTROLLER_TIMERS_H

#define MOTOR_PULSE PIND4
#define MOTOR_PULSE_DD DD4

void init_timers(void);
void init_counter_port(void);
void init_counter_timer(void);
void init_frequency_timer(void);

uint8_t motor_frequency(void);
void clear_elapsed_timer(void);
bool elapsed_timer_greater_than(uint16_t limit);

#endif //GRINDER_CONTROLLER_TIMERS_H
