#include <Arduino.h>
#include <Arduino/Interrupts>
#include "pid.h"
#include "motor.h"


#define OUTPUT_MAX 255
#define OUTPUT_MIN -255

//can only have one pid, but we only have one motor so that's ok
static PID* pid_ptr;

PID::PID(double kp, double ki, double kd, double sampleTime, double scaling_factor){
	this->kp = kp;
	this->ki = ki;
	this->kd = kd;
	this->sampleTime = sampleTime;
    this->scaling_factor = scaling_factor;
	pid_output=0;
	pid_ptr = this;
	timer_init();
}

/*
	range -255....255
*/
double PID::calculate_speed(int16_t pos){
	static int16_t last_pos = pos;
    long int rspeed = pos-last_pos;
    //printf("rspeed: %ld \n",rspeed);
	double speed = rspeed/sampleTime;
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
void PID::Compute(void){
	static double Integrator_term;
    static int i;
    double my_output;
	//scale error to output: -255 ...255
    //printf("ref, measure       %d %d \n", reference,int(measurement) );
    double ref  = reference;
	double error = (ref * 2.52 ) - (measurement);
	double proportional = (kp * error);
	
	Integrator_term += ki*error*sampleTime ;
	// limit the integration term
	if(Integrator_term > OUTPUT_MAX) Integrator_term = OUTPUT_MAX;
	else if(Integrator_term < OUTPUT_MIN) Integrator_term = OUTPUT_MIN;
	my_output = proportional + Integrator_term;
	// limit the output term
	if(my_output > OUTPUT_MAX) my_output = OUTPUT_MAX;
	else if(my_output < OUTPUT_MIN) my_output = OUTPUT_MIN;
    if (i++%250==0){
        printf("error prop, inte, output      %d %d  %d %d\n", int(error), int(proportional), int(Integrator_term), int(my_output));
    }
    
	//output should be between -255..255
	pid_output = my_output;
    //printf("Output: %d\n", pid_output);
}

void PID::update_reference(int16_t newreference){
	this->reference = newreference;
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
	TIMSK4 |= (1 << TOIE4);
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
	int16_t speed = motor_read();
    pid_ptr->measurement=speed/13;
    pid_ptr->Compute();
    interrupts();
    motor_set_speed(pid_ptr->pid_output);
    
	/*
    double speed = pid_ptr->calculate_speed(pos);
    if (max_speed < speed){
        max_speed=speed;
    }
    if (++i%10==0){
        printf("max_speed %d\n", int(max_speed));
    }        
	/*pid_ptr->measurement = speed;
	pid_ptr->Compute();
	interrupts();
	motor_set_speed(pid_ptr->pid_output);*/
}