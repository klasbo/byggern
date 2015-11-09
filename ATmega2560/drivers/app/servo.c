


#include "servo.h"
#include "../core/pwm/pwm.h"

static float min;
static float max;

void servo_init(float _min, float _max){
    pwm_period(20.0f);
    min = _min;
    max = _max;
}

void servo_write(int8_t val){
    pwm_width(((float)val + 128.0) * (max-min) / 256.0  +  min);
}