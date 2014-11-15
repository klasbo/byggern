#include <Arduino.h>
#include <Arduino/Interrupts>
#include <inttypes.h>
#include "pid.h"
#include "motor.h"


static PID* pid_ptr;

PID::PID(double kp, double ki, double kd, int16_t low_limit, int16_t high_limit, double sampleTime ){
	NoInterrupts();
	this->kp = kp;
	this->ki = ki;
	this->kd = kd;
	this->sampleTime = sampleTime;
	outMax = high_limit;
	outMin = low_limit;
	pid_ptr = this;
	timer_init();

	Interrupts();
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

void PID::timer_shutdown(void){}

/*
	maxspeed ~32500
	speed -100...100
	output -255...255
	=> ~161 motor_counter for each speed
	=< ~255 motor_counter per output
*/
int16_t PID::Compute(void){
	static int16_t output;
	static int16_t lastInput;
	static double Integrator_term;
	
	int16_t error = reference - input;
	Integrator_term += ( ki*error );

	// limit the integration term
	if(Integrator_term > outMax) Integrator_term= outMax;
	else if(Integrator_term < outMin) Integrator_term= outMin;

	// calculate output:
	// proportional part + integral part*time + derivative part/time
	int16_t output = kp * error + Integrator_term - /*kd*(input - lastInput)/sampleTime*/;
		
		
	// limit the output term
	if(output > outMax) output = outMax;
	else if(output < outMin) output = outMin;
	//lastInput = input;

	//output should be between -255..255
	return pid_output;
}

//clock 5 is occupied by servo library
//using clock 4 should be safest



void PID::timer_init(void){
	//NoInterrupts();
	//set everything to zero
	TCCR4A	= 0; //normal counting mode. we want to use overflow interrupt
	TCCR4B	= 0;
	TCNT4	= 0;

	//256 prescaler
	TCCR4B |= (1<<CS42);

	TIMSK5 |= (1 << TOIE5);


	/*
	TCNT4 = 0;
	OCR4A = (16*10^6) / (time_frequency*PRESCALER)-1; //must be less than 16^2 = 65536
	TCCR4A |= (1 << WGM42); // turn on CLC mode
	TCCR4B |= ; //sets the prescaler to some number see datasheet page 157
	TIMSK4 |= (1 << OCIE4A); //enable clc interrupt
	*/
	//Interrupts();
}

ISR( TIMER4_OVF_vect ){
	//convert sampletime from seconds to milliseconds
	//set the counter down to some value
	TCNT4 = 65536 -  int((pid_ptr->sampleTime*1000)*62.5);

	pid_ptr->Calculate();
	motor_set_speed(pit_ptr->pid_output);

}