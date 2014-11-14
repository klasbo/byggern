#pragma once

//time in milliseconds
class PID;

class PID{
	public:
	PID(double kp,double ki,double kd,int16_t low_limit,int16_t high_limit,double sampleTime );

	int16_t Compute(int16_t input,int16_t reference);
	int16_t calculate_speed(int16_t pos);
	
	private:
	double kp;
	double ki;
	double kd;
	double sampleTime;
	int16_t outMin, outMax;
};



