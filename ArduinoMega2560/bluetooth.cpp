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
    //running=isRunning;
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

void BT_stop(void){
    noInterrupts();
    //turn off prescaler and overflow interrupts
    TCCR3B = 0;
    TIMSK3 = 0;
    *running=-1;
    Serial.end();
    interrupts();
}

int16_t coerce(int16_t val, int16_t min, int16_t max){
    if(val > max){
        return max;
    } else if(val < min){
        return min;
    } else {
        return val;
    }
}

void BT_run(Servo* s){
    
    int ledPin = 13;//for testing
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    char key= ' ';
    int	speed_r = 100;
    int speed_l = -100;
    static int16_t servo_angle = 90;
    const int16_t servo_angle_max = 160;
    const int16_t servo_angle_min = 20;
    int servo_adjusted = 0;
    if(Serial.available() ){
        key = Serial.read();
        printf("%c\n", key);
    }
    
    switch(key){
        case 'w':
            digitalWrite(ledPin, HIGH);
            break;
        case 's':
            digitalWrite(ledPin, HIGH);
            break;
        case 'a':
            interrupts();
            motor_set_speed(speed_l);
            noInterrupts();
            break;
        case 'd':
            interrupts();
            motor_set_speed(speed_r);
            noInterrupts();
            break;
        case 'r':
            servo_angle -= 20;
            servo_angle = coerce(servo_angle, servo_angle_min, servo_angle_max);
            s->write(servo_angle);
            break;
        case 'l':
            servo_angle += 20;
            servo_angle = coerce(servo_angle, servo_angle_min, servo_angle_max);
            s->write(servo_angle);
            break;
        case 'p':
            printf("p\n");
            digitalWrite(7,1);        
            break;
        case 'q':
            BT_stop();
            break;
        case 'x': // button is released
            interrupts();
            motor_set_speed(0);
            noInterrupts();
            digitalWrite(ledPin, LOW);
            digitalWrite(7,0);
            break;
    }        
}

ISR(TIMER3_OVF_vect){
    TCNT3 = 65536 - int(sampleTime*62.5*1000);
    BT_run(s);
}