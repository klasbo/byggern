#pragma once

#include <stdint.h>

//time in milliseconds
class PID;

class PID{
	public:
	PID( double kp, double ki, double kd, double sampleTime, double scaling_factor );

	void Compute(void);
	double calculate_speed(int16_t pos);
	void update_reference(int16_t reference);
	~PID();
	
	void timer_init(void);
	void timer_shutdown();
    
	double scaling_factor;
	double kp;
	double ki;
	double kd;
	int16_t reference;
	double measurement;
	int16_t pid_output;
	double sampleTime;
};



