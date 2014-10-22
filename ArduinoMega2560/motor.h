#pragma once
#include <stdint.h>

void motor_init(void);


void motor_write(uint8_t speed, uint8_t dir);

uint8_t motor_read(void);



/*
void motor_init(void){
    Wire.begin();
    Serial.begin(9600);
    DDRF |= (1<<PINF5)|(1<<PINF6)|(1<<PINF2)|(1<<PINF3)|(1<<PINF4);
    //PORTF |= (1<<PINF4); //select low to read MSB
    //PORTF |= (1<<PINF3); //!reset
    
    
    //pinMode(A6,OUTPUT);
    //pinMode(PINF7,OUTPUT);
    //digitalWrite(92, HIGH); //motor enable
    //digitalWrite(91, HIGH); //Direction
    
    //digitalWrite(//activate output of encoder
    
    
    
}

void motor_write(uint8_t data, uint8_t dir){
    if (dir == 1){
        PINF |= (1 << PINF6);
        } else {
        PINF &= ~(1 << PINF6);
    }
    PINF |= (1<<PINF5); //motor ENABLE
    Wire.beginTransmission(0b00101111);
    Wire.write(0x00);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t motor_read(void){
    uint8_t val = 0;
    PORTF |= (1<<PINF3); //!RST
    PORTF &= ~(1<<PINF2); //enable read encoder
    PORTF |= (1<<PINF4); //select low to read MSB
    delayMicroseconds(20);
    val = PINK;
    
    PORTF &= ~(1<<PINF3); //!RST
    PORTF |= (1<<PINF3); //!RST
    
    PORTF |= (1<<PINF2); // disable read encoder
    return PINK;
    
}

*/