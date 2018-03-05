//
// Created by Garth Webb on 2/27/18.
//

#ifndef GRINDER_CONTROLLER_MOTOR_H
#define GRINDER_CONTROLLER_MOTOR_H

#define MOTOR_CTRL PORTC0
#define MOTOR_CTRL_DD DD0
#define MOTOR_CTRL_ON PORTC |= _BV(MOTOR_CTRL)
#define MOTOR_CTRL_OFF PORTC &= ~_BV(MOTOR_CTRL)

void init_motor(void);
void start_motor(void);
void stop_motor(void);

#endif //GRINDER_CONTROLLER_MOTOR_H
