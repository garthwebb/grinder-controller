//
// Created by Garth Webb on 2/27/18.
//

#ifndef GRINDER_CONTROLLER_STATE_MACHINE_H
#define GRINDER_CONTROLLER_STATE_MACHINE_H

// Because we sample every 10ms, this is decahertz (Frequency/10)
#define MIN_FREE_FREQUENCY 188
#define MIN_SAFE_FREQUENCY 100

// Amount of time in 1/10th seconds we should be in the starting step before
// determining if we're in a good state.
#define STARTING_TIME 10

// Amount of time in 1/10th seconds we should be in the auto run finish step
#define AUTO_FINISH_TIME 30

void evaluate_state(void);

void handle_auto_run_starting(void);
void handle_auto_run(void);
void handle_auto_run_finishing(void);
void handle_manual_run_starting(void);
void handle_manual_run(void);
void handle_manual_run_clearing(void);
void handle_stopped(void);

void transition_auto_run_starting(void);
void transition_auto_run(void);
void transition_auto_run_finishing(void);
void transition_manual_run_starting(void);
void transition_manual_run(void);
void transition_manual_run_clearing(void);
void transition_stopped(void);

#endif //GRINDER_CONTROLLER_STATE_MACHINE_H
