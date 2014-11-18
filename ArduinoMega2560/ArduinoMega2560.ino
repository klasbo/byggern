#include "can.h"
#include "spi.h"
#include "mcp2515.h"
#include "ir.h"
#include "uart.h"
#include "motor.h"

#include <stdio.h>
#include <Arduino.h>
#include <Servo/Servo.h>
#include <Wire/Wire.h>
#include "bluetooth.h"


#include "can_types.h"


#define CAN_JOYSTICK_ID 3


int main(void){
    extern void init(void);
    init();
    uart_init();
    SPI_init();
    CAN_init();
    IR_init(); // not necessary?

    motor_init();

    // Solenoid pin
    pinMode(solenoidPin, OUTPUT);

    Servo s;
    s.attach(6);

    BT_init(&s, 0.05);


    can_msg_t   recvMsg;
    ControlCmd  cmd;
    uint8_t     game_over;

    int16_t i=0;

    while(1){
        recvMsg     = CAN_recv();
        game_over   = IR_obstructed();

        switch(recvMsg.ID){
            case CANID_ControlCmd:
                memcpy(&cmd, recvMsg.data, recvMsg.length);
                
                motor_set_speed(cmd.motorSpeed);
                
                s.write(cmd.servoPos);
                
                digitalWrite(7, cmd.solenoid ? 1 : 0);

                
                break;
            default:
                break;
        }
        if(game_over){
            can_msg_t m;
            m.ID = CANID_GameOver;
            m.length = 0;
            CAN_send(m);
        }
    }
}


