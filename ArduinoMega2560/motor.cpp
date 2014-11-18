#include <Arduino.h>
#include <interrupt.h>
#include <Wire/Wire.h>
#include "motor.h"




void motor_init(void){
    Wire.begin();
    DDRF    |=  (1<<DDF2)   // Output encoder enable
            |   (1<<DDF3)   // Counter reset
            |   (1<<DDF4)   // Encoder LSB/MSB select
            |   (1<<DDF5)   // Motor enable
            |   (1<<DDF6);  // Motor direction
            
    DDRK = 0;
    PORTF &= ~(1 << PF2);
    PORTF |= (1 << PF3);
    PORTF &= ~(1 << PF4);
    PORTF |= (1 << PF5);
}

void motor_write(uint8_t speed, uint8_t dir){
    if (dir == right){
        PORTF |=  (1 << PF6);
    } else {
        PORTF &= ~(1 << PF6);
    }
    Wire.beginTransmission(0b00101111); 
        Wire.write(0x00);
        Wire.write(speed);
    Wire.endTransmission();
}

uint16_t motor_read(void){
    int8_t hsb = PINK;
    hsb = ((hsb & 0x55) << 1) | ((hsb & 0xaa) >> 1);
    hsb = ((hsb & 0x33) << 2) | ((hsb & 0xcc) >> 2);
    hsb = ((hsb & 0x0f) << 4) | ((hsb & 0xf0) >> 4);
    
	PORTF |= (1 << PF4);
	delayMicroseconds(20);
	
	int8_t lsb = PINK;
	lsb = ((lsb & 0x55) << 1) | ((lsb & 0xaa) >> 1);
	lsb = ((lsb & 0x33) << 2) | ((lsb & 0xcc) >> 2);
	lsb = ((lsb & 0x0f) << 4) | ((lsb & 0xf0) >> 4);
	
    
    
	PORTF &= ~(1 << PF4);
	int16_t x = (hsb << 8) + lsb;
    
    PORTF &= ~(1 << PF3);
    delayMicroseconds(20);
    PORTF |= (1 << PF3);
    
    return x;
}

/** Speed is in range [-100, 100]
*/
void motor_set_speed(int speed){
	motor_write((uint8_t)(abs(speed) * 255 / 100), speed > 0 ? right : left);
}