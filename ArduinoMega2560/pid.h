#pragma once

//time in milliseconds
class PID;

class PID{
	public:
	PID(double kp,double ki,double kd,int16_t low_limit,int16_t high_limit,double sampleTime );

	int16_t Compute(void);
	int16_t calculate_speed(int16_t pos);
	void update_reference(int16_t reference);
	void update_measurement(int16_t measurement);
	
	private:
	void timer_init(void);
	void timer_shutdown();
	double kp;
	double ki;
	double kd;
	int16_t reference;
	int16_t measurement;
	int16_t pid_output;

	double sampleTime;
	int16_t outMin, outMax;
};



