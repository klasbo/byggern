
#include "motor.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>

#include "max520.h"
#include "../../config.h"
#include "../../../lib/macros.h"

#define MJ1 C
#define MJ2 K

#define OE  2
#define RST 3
#define SEL 4
#define EN  5
#define DIR 6

#define PIN(x)  concat(PIN, x)
#define DDR(x)  concat(DDR, x)
#define PORT(x) concat(PORT, x)

#define motor_vel_max 0.7

float saturate(float f, float min, float max){
    return  f > max ? max :
            f < min ? min : 
                      f;
}

static inline void motor_enable(uint8_t enable){
    enable ? 
        (PORT(MJ1) |= (1 << EN)) :
        (PORT(MJ1) &= ~(1 << EN));
}

static inline void motor_encoder_output_enable(uint8_t enable){
    enable ? 
        (PORT(MJ1) &= ~(1 << OE)):
        (PORT(MJ1) |= (1 << OE)) ;
}

void motor_encoderReset(void){
    PORT(MJ1) &= ~(1 << RST);
    _delay_us(20);
    PORT(MJ1) |= (1 << RST);
}

typedef enum EncoderByte EncoderByte;
enum EncoderByte {
    EB_high = 0,
    EB_low
};

static inline void motor_encoder_select_byte(EncoderByte e){
    switch(e){
    case EB_high:   PORT(MJ1) &= ~(1 << SEL);   break;
    case EB_low:    PORT(MJ1) |= (1 << SEL);    break;
    default: break;
    }
    _delay_us(20);
}

static inline uint8_t reverse_bits(uint8_t x){
    x = ((x & 0x55) << 1) | ((x & 0xaa) >> 1);
    x = ((x & 0x33) << 2) | ((x & 0xcc) >> 2);
    x = ((x & 0x0f) << 4) | ((x & 0xf0) >> 4);
    return x;
}



void motor_init(void){
    max520_init(MAX520_TWI_ADDR);

    DDR(MJ1)    |=  (1<<OE)
                |   (1<<RST)
                |   (1<<SEL)
                |   (1<<EN)
                |   (1<<DIR);
            
    DDR(MJ2) = 0;
    motor_encoder_output_enable(1);
    motor_encoderReset();
    motor_encoder_select_byte(EB_high);
    motor_speed(0);
    motor_enable(1);
}



void motor_direction(MotorDirection dir){
    switch(dir){
    case left:  PORT(MJ1) &= ~(1 << DIR);   break;
    case right: PORT(MJ1) |= (1 << DIR);    break;
    default: break;
    }
}

void motor_speed(uint8_t speed){
    max520_write(MAX520_CHANNEL_MOTOR, speed);
}

void motor_velocity(float vel){
    vel = saturate(vel, -motor_vel_max, motor_vel_max);
    motor_speed((uint8_t)(fabs(vel) * 250.0));
    motor_direction(vel < 0 ? left : right);
}


int16_t motor_encoderRead(void){
    motor_encoder_output_enable(1);

    motor_encoder_select_byte(EB_high);
    uint8_t msb = reverse_bits(PIN(MJ2));
    motor_encoder_select_byte(EB_low);
    uint8_t lsb = reverse_bits(PIN(MJ2));
        
    motor_encoder_output_enable(0);

    return -((msb << 8) | lsb);
}
