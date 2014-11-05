#include <stdint.h>
#include <arduino.h>
#include "pid.h"


int16_t PID::calculate_speed(int16_t pos){
    static int16_t speed;
    unsigned long now = millis();
    if (now - lastTime >= sampleTime){
        speed = ((pos - last_pos)*sampleTime)/1000;
        return speed;
    }
    return speed
}

//time in milliseconds
PID::PID(   int16_t kp,
int16_t ki,
int16_t kd
int16_t lower_limit
int16_t higher_limit
int16_t sampletime){

    Kp = kp;
    Ki = ki;
    Kd = kd;
    sampleTime = sampletime;
    lastTime = millis() - SampleTime();
    lastOutput = 0;
    lastInput = 0;
}

int16_t PID::compute(int16_t input,int16_t reference){
    static int16_t output;
    unsigned long now = millis();
    if (now - lastTime >= sampleTime){
        int16_t error = reference - input;
        Integrator_term += ( Ki*error );

        // limit the integration term
        if(ITerm > outMax) Integrator_term= outMax;
        else if(ITerm < outMin) Integrator_term= outMin;

        // calculate output:
        // proportional part + integral part*time + derivative part/time
        int16_t output = Kp * error + I_Term*(sampleTime / 1000) - Kd*(input - lastInput)*(1000 / sampleTime);
        

        // limit the output term
        if(output > outMax) output = outMax;
        else if(output < outMin) output = outMin;

        lastInput = input;
        lastTime = now;
        return output;
    }
    else{
        return output;
    }
}

#include <avr/interrupts>
#include <inttypes.h>

#define PRESCALER 64
#define time_frequency 1000

static volatile unsigned long time;

//using clock 5
void timer_init(void){
    cli();
    TCNT5 = 0;
    OCR5A = (16*10^6) / (time_frequency*PRESCALER)-1; //must be less than 16^2 = 65536
    TCCR5A |= (1 << WGM52); // turn on CLC mode
    TCCR5B |= ; //sets the prescaler to some number see datasheet page 157
    TIMSK5 |= (1 << OCIE5A); //enable clc interrupt
}

uint16_t getTime(void){
    cli();
    uint16_t mytime = time;
    sei();
    return time;
}

ISR( TIMER5_COMPA_vekt ){
    cli();
    ++time;
    sei();
}