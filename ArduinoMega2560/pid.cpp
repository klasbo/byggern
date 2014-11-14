#include <Arduino.h>
#include <inttypes.h>
#include "pid.h"

PID::PID(double kp, double ki, double kd, int16_t low_limit, int16_t high_limit, double sampleTime ){
	this->kp = kp;
	this->ki = ki;
	this->kd = kd;
	this->sampleTime = sampleTime;
	outMax = high_limit;
	outMin = low_limit;
}


int16_t PID::calculate_speed(int16_t pos){
	static int16_t speed;
	static int16_t last_pos = pos;
	static unsigned long lastTime=millis();
	unsigned long now = millis();
	if (now - lastTime >= sampleTime*1000){
		speed = (pos - last_pos)/sampleTime;
		//printf("abs_speed %d\n", pos-last_pos);
		last_pos = pos;
		lastTime = now;
		return speed;
	}
	return speed;
}

//maxspeed ~32500
//speed [0....100]
// => ~320 motor_counter for each speed
int16_t PID::Compute(int16_t input,int16_t reference){
	static int16_t output;
	static int16_t lastInput;
	unsigned long now = millis();
	unsigned static long lastTime = millis();
	static double Integrator_term;
	
	if (now - lastTime >= sampleTime*1000){
		if(now - lastTime >= sampleTime*2000){
			printf("PID running slow, missing deadlines\n");
		}
		int16_t error = reference - input;
		Integrator_term += ( ki*error );

		// limit the integration term
		if(Integrator_term > outMax) Integrator_term= outMax;
		else if(Integrator_term < outMin) Integrator_term= outMin;

		// calculate output:
		// proportional part + integral part*time + derivative part/time
		int16_t output = kp * error + Integrator_term - kd*(input - lastInput)/sampleTime;
		
		
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
/*
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
}*/