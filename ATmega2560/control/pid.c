#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "pid.h"

struct PID_controller {
    float reference;
    
    float Kp;
    float Kd;
    float Ki;
    
    Timer timer;    
    
    float prev_error;
    float integral;
};


PID_controller* pid_new(float Kp, float Ki, float Kd, Timer timer){
    PID_controller* c = malloc(sizeof(PID_controller));    
    memset(c, 0, sizeof(PID_controller));
    
    c->Kp = Kp;
    c->Ki = Ki;
    c->Kd = Kd;
        
    c->timer = timer;
    
    return c;
}

void pid_reference(PID_controller* c, float ref){
    c->reference = ref;
}

float pid_actuate(PID_controller* c, float y){
    
    float dt = ((float)*(c->timer.source) * c->timer.prescaler) / F_CPU;
    *c->timer.source = 0;
    
    float error         = c->reference - y;
    c->integral         += error * dt;
    float derivative    = (error - c->prev_error) / dt;
    c->prev_error       = error;
        
    return      c->Kp * error +
                c->Ki * c->integral +
                c->Kd * derivative;
}



