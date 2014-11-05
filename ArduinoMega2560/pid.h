#pragma once

//time in milliseconds
class PID{
    public:
    PID(    int16_t kp,
    int16_t ki,
    int16_t kd
    int16_t lower_limit
    int16_t higher_limit
    int16_t sampletime );

    int16_t compute(int16_t input,int16_t reference);
    int16_t calculate_speed(int16_t motor_pos);
    private:
    int16_t kp;
    int16_t ki;
    int16_t kd;
    int16_t lastTime;
    int16_t Integrator_term;
    int16_t lastInput;
    int16_t sampleTime;
    int16_t outMin, outMax;
    int16_t last_motor_pos;
}
