#include <Arduino.h>
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
    
}

void motor_write(uint8_t data, uint8_t dir){
    if (dir == 1){
        PINF |=  (1 << PINF6);
    } else {
        PINF &= ~(1 << PINF6);
    }
    PINF |= (1<<PINF5); // Enable motor
    Wire.beginTransmission(0b00101111); 
        Wire.write(0x00);
        Wire.write(data);              
    Wire.endTransmission();    
}

uint8_t motor_read(void){
    
    /*
    uint8_t val = 0;
    PORTF |= (1<<PINF3); //!RST
    PORTF &= ~(1<<PINF2); //enable read encoder 
    PORTF |= (1<<PINF4); //select low to read MSB
    delayMicroseconds(20);
    val = PINK;
    
    PORTF &= ~(1<<PINF3); //!RST
    PORTF |= (1<<PINF3); //!RST
    
    PORTF |= (1<<PINF2); // disable read encoder
    */
    
    printf("PINK: %2x\n", PINK);
    uint8_t x = PINK;
    x = ((x & 0x55) << 1) | ((x & 0xaa) >> 1);
    x = ((x & 0x33) << 2) | ((x & 0xcc) >> 2);
    x = ((x & 0x0f) << 4) | ((x & 0xf0) >> 4);
    
   return x;
    
    /*
    //PORTF &= ~(1<<PINF2); // enable read encoder
    uint8_t val = PINK;
    PORTF &= ~(1<<PINF3); // Reset toggle pulse
    PORTF |=  (1<<PINF3); // ditto
    //PORTF |=  (1<<PINF2); // disable read encoder   
    
    return val;
    */
}

