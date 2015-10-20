
#include "motor.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "max520.h"
#include "../../config.h"


static inline void motor_enable(uint8_t enable){
    enable ? 
        (PORTC |= (1 << PC5)) :
        (PORTC &= ~(1 << PC5));        
}

static inline void motor_encoder_output_enable(uint8_t enable){
    enable ? 
        (PORTC &= ~(1 << PC2)):  
        (PORTC |= (1 << PC2)) ;
}

void motor_encoder_counter_reset(void){
    PORTC &= ~(1 << PC3);
    _delay_us(20);
    PORTC |= (1 << PC3);
}

typedef enum EncoderByte EncoderByte;
enum EncoderByte {
    EB_high = 0,
    EB_low
};

static inline void motor_encoder_select_byte(EncoderByte e){
    switch(e){
    case EB_high:   PORTC &= ~(1 << PC4);   break;
    case EB_low:    PORTC |= (1 << PC4);    break;
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

    DDRC    |=  (1<<DDC2)   // Output encoder enable
            |   (1<<DDC3)   // Counter reset
            |   (1<<DDC4)   // Encoder LSB/MSB select
            |   (1<<DDC5)   // Motor enable
            |   (1<<DDC6);  // Motor direction
            
    DDRK = 0;
    motor_encoder_output_enable(1);
    motor_encoder_counter_reset();
    motor_encoder_select_byte(EB_high);
    motor_enable(1);
}



void motor_direction(MotorDirection dir){
    switch(dir){
    case left:  PORTC &= ~(1 << PC6);   break;
    case right: PORTC |= (1 << PC6);    break;
    default: break;
    }
}

uint8_t buf[2] = {0};

void motor_speed(uint8_t speed){
    max520_write(MAX520_CHANNEL_MOTOR, speed);
}


int16_t motor_encoder_read(void){
    motor_encoder_output_enable(1);

    motor_encoder_select_byte(EB_high);
    uint8_t msb = reverse_bits(PINK);
    motor_encoder_select_byte(EB_low);
    uint8_t lsb = reverse_bits(PINK);
        
    motor_encoder_output_enable(0);

    return -((msb << 8) | lsb);
}