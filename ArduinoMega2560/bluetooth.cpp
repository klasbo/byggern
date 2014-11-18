#include <Arduino.h>
#include <interrupt.h>
#include "bluetooth.h"
#include "motor.h"

static double sampleTime;
static Servo* s;
static int16_t* running;


void BT_init(Servo* servo, double sampletime){
    sampleTime = sampletime;
    s = servo;
    Serial.begin(9600);
    BT_enable_interrupts();
    printf("start\n");
}

void BT_enable_interrupts(void){
    noInterrupts();
    //normal operation mode
    TCCR3A = 0;
    TCCR3B = 0;
    TCNT3 = 0;
    
    //256 prescaler
    TCCR3B |= (1 << CS32);
    
    //enable overflow interrupts
    TIMSK3 |= (1 << TOIE3);
    interrupts();
}

int16_t coerce(double val, double min, double max){
    if(val > max){
        return max;
    } else if(val < min){
        return min;
    } else {
        return val;
    }
}

void BT_run(Servo* s){
    char key= ' ';
    int	speed_r = 100;
    int speed_l = -100;
    static double servo_angle = 90;
	const double servoangle_per_time = 50;
    const double servo_angle_max = 160;
    const double servo_angle_min = 20;
    int servo_adjusted = 0;
	static int servo_moving;
    if(Serial.available() ){
        key = Serial.read();
        printf("%c\n", key);
    }
    
    switch(key){
        case 'a': // drive motor left
            interrupts();
            motor_set_speed(speed_l);
            noInterrupts();
            break;
        case 'd': // drive motor right
            interrupts();
            motor_set_speed(speed_r);
            noInterrupts();
            break;
        case 'r': // servo right
			servo_moving = 1;
            break;
        case 'l': // servo left
			servo_moving = -1;	
            break;
        case 'p': // solenoid
            digitalWrite(solenoidPin,1);        
            break;
        case 'x': // button is released
            interrupts();
            motor_set_speed(0);
            noInterrupts();
            digitalWrite(solenoidPin,0);
            break;
		case 'z': // servo is released
			servo_moving = 0;
			break;
	}
	if(servo_moving == 1){ // if servo moving right
		servo_angle -= servoangle_per_time*sampleTime;
		servo_angle = coerce(servo_angle, servo_angle_min, servo_angle_max);
		s->write(int(servo_angle));
		
	}
	else if(servo_moving == -1){ // if servo moving left
		servo_angle += servoangle_per_time*sampleTime;
		servo_angle = coerce(servo_angle, servo_angle_min, servo_angle_max);
		s->write(int(servo_angle));
	}
}

ISR(TIMER3_OVF_vect){
    TCNT3 = 65536 - int(sampleTime*62.5*1000);
    BT_run(s);
}