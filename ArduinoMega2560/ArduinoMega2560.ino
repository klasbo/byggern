
#include <stdio.h>
#include <Arduino.h>
#include <Servo/Servo.h>
#include <Wire/Wire.h>

#include "can.h"
#include "ir.h"
#include "uart.h"
#include "motor.h"
#include "bluetooth.h"
#include "pin_config.h"
#include "can_types.h"


int main(void){
    extern void init(void);
    init();
    uart_init();
    CAN_init();
    IR_init();

    motor_init();

    pinMode(solenoidPin, OUTPUT);

    Servo s;
    s.attach(servoPin);

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
                
                digitalWrite(solenoidPin, cmd.solenoid ? 1 : 0);

                
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


