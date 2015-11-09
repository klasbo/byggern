#pragma once


#include "timer.h"

typedef struct PID_controller PID_controller;

// Allocates a new generic-ish PID controller
PID_controller* pid_new(float Kp, float Ki, float Kd, Timer timer);

// Sets the reference point of the controller
void pid_reference(PID_controller* c, float ref);

// Generates a control input (u) given a new measurement (y)
// Periodic execution is preferable but not necessary, since
// the PID_controller stores the time it was last updated
// For this reason, this function assumes that the time between 
// two successive calls is less than however long it is for 
// the timer to wrap around
float pid_actuate(PID_controller* c, float y);
