#include <Arduino.h>
#include <Arduino/Interrupts>
#include <inttypes.h>
#include "pid.h"
#include "motor.h"

#define OUTPUT_MAX 255
#define OUTPUT_MIN 255

//can only have one pid, but we only have one motor so that's ok
static PID* pid_ptr;

PID::PID(double kp, double ki, double kd, double sampleTime, double scaling_factor){
	this->kp = kp;
	this->ki = ki;
	this->kd = kd;
	this->sampleTime = sampleTime;
	pid_output=0;
	pid_ptr = this;
	timer_init();
}

/*
	returns real speed


*/
int16_t PID::calculate_speed(int16_t pos){
	static int16_t speed;
	static int16_t last_pos = pos;
	speed = (pos - last_pos)/sampleTime;
	last_pos = pos;
	return speed;
}

/*
	maxspeed +- ~32500 motor_counters 
	//dont want user to input maxspeed (too fast). limit to 180*32500/255 = 22950?
	speed -100...100
	output -255...255
	=> ~321 motor_counter for each speed at 32500, or 227
	=> ~127 motor_counter per output
*/
int16_t PID::Compute(void){
	static double Integrator_term;
	int16_t my_output;
	
	//scale error to output: -255 ...255
	int16_t error = int16_t(reference * 2.52 * scaling_factor) - int16_t(measurement/127.0);
	int16_t proportional = int16_t(kp * error);
	
	
	Integrator_term += int( (ki*error) * sampleTime) ;
	printf("inte, ki, error, sampletime %d, %d, %d, %d\n", int16_t(Integrator_term), int16_t(ki*1000), int16_t(error), int16_t(sampleTime*1000));
	// limit the integration term
	if(Integrator_term > OUTPUT_MAX) Integrator_term = OUTPUT_MAX;
	else if(Integrator_term < OUTPUT_MIN) Integrator_term = OUTPUT_MIN;
	printf("inte %d\n", Integrator_term);
	
	//my_output = int16_t(proportional) + int16_t(Integrator_term);	
	printf("output, prop, inte   %d %d %d \n",(my_output), (proportional),(Integrator_term));
	
	// limit the output term
	if(my_output > OUTPUT_MAX) my_output = OUTPUT_MAX;
	else if(my_output < OUTPUT_MIN) my_output = OUTPUT_MIN;

	//output should be between -255..255
	pid_output = my_output;
	printf("error, prop, integrator      %d, %d, %d   \n",error,int16_t(proportional), Integrator_term);
	printf("myoutput, pid_output: %d %d\n",my_output, pid_output);
}

void PID::update_reference(int16_t newreference){
	reference = newreference;	
}

PID::~PID(){
	timer_shutdown();
}

/* 
	clock 5 is occupied by servo library
	using clock 4 should be safest
 */
void PID::timer_init(void){
	noInterrupts();
	// set everything to zero
	TCCR4A	= 0; //normal counting mode. we want to use overflow interrupt
	TCCR4B	= 0;
	TCNT4	= 0;

	// 256 prescaler
	TCCR4B |= (1<<CS42);

	// enable overflow interrupts
	TIMSK4 |= (1 << TOIE5);
	interrupts();
}


void PID::timer_shutdown(void){
	noInterrupts();
	TCCR4B = 0; // turn off prescaler
	TIMSK4 = 0; // turn off interrupts from clock 4
	interrupts();
}


ISR( TIMER4_OVF_vect ){
	// convert sampletime from seconds to milliseconds
	// set the counter down to some value so that it will count up to top again
	// in sampleTime seconds.
	TCNT4 = 65536 -  int((pid_ptr->sampleTime*1000)*62.5);
	printf("ISR\n");
	int16_t pos = motor_read();
	int16_t speed = pid_ptr->calculate_speed(pos);
	pid_ptr->measurement = speed;
	pid_ptr->Compute();
	interrupts();
	printf("l\n");
	motor_set_speed(pid_ptr->pid_output);
	printf("q\n");
}