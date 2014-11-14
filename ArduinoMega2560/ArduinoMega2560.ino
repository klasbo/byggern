#include "can.h"
#include "spi.h"
#include "mcp2515.h"
#include "led.h"
#include "uart.h"
#include "motor.h"
#include "analog_info.h"

#include <stdio.h>
#include <Arduino.h>
#include <Servo/Servo.h>
#include <Wire/Wire.h>
#include "pid.h"


#include "can_types.h"


void can_test(void);



#define CAN_JOYSTICK_ID 3

struct JOY_pos_t {
    int8_t x;
    int8_t y;
};

struct JOY_pos_t CAN_to_joystick(can_msg_t msg){
    JOY_pos_t p;
    p.x = msg.data[0];
    p.y = msg.data[1];
    return p;
}



int main(void){
    extern void init(void);
    init();

    uart_init();	
    SPI_init();
    CAN_init();
    LED_init(); // not necessary?

	motor_init();
	pinMode(7, OUTPUT);
	//motor_range test = motor_calibrate();
	//delay(2000);
	//printf("max left and right: %d %d\n",test.max_l, test.max_r);


	PID mypid(1,1,0, -255, 255, 0.10);
    Servo s;
    s.attach(6);
    can_msg_t   msg;
	ControlCmd  cmd;
	int16_t position;
	int16_t maxSpeed=0;
	int16_t speed=0;
	int16_t reference_speed=0;
	int16_t u = 0;
	while(1){
		msg	 = CAN_recv_blocking();
		position = motor_read();
		speed =mypid.calculate_speed(position);
		u = mypid.Compute(speed, reference_speed);
		printf("u %d\n",u );
		motor_set_speed(cmd.motorSpeed);
        switch(msg.ID){
        case CANID_ControlCmd:
            memcpy(&cmd, msg.data, msg.length);
            //printf("received can ControlCmd: (%d, %d, %d)\n", cmd.motorSpeed, cmd.servoPos, cmd.solenoid);
			reference_speed = cmd.motorSpeed;
            //motor_set_speed(cmd.motorSpeed);
            s.write(cmd.servoPos);
            digitalWrite(7, cmd.solenoid ? 1 : 0);
        }
	}

	
	
	/*
    for(int pos = 0; pos < 180; pos += 1){
        s.write(pos);
        delay(15);
    }
    for(int pos = 180; pos>=1; pos-=1){
        s.write(pos);
        delay(15);
    }
    
    s.write(90);
    */

	/*
    while(1){
        //printf("\n\nled signal: %d\n\n", LED_read());
        printf("Score: %d\n\n", LED_score_count() );
        //delay(1000);
    }*/
    
	/*
    can_msg_t msg;
    int iter = 0;
    while(1){
		delay(500);
        printf("\niter %4d\n\n", iter++);
        
        can_msg_t msg2 = CAN_recv_blocking();
        switch(msg2.ID){
        case 0: break;
        case CAN_JOYSTICK_ID:
            printf("Received joystick position: (%d, %d)", CAN_to_joystick(msg2).x, CAN_to_joystick(msg2).y);
            break;
        case 10:
            printf("Received string: %s\n", msg2.data);
            break;
        default:
            printf("Received unknown msg:\n  can_msg_t(id:%d, len:%d, data:(%d, %d, %d, %d, %d, %d, %d, %d))\n\n",
                msg2.ID,
                msg2.length,
                msg2.data[0], msg2.data[1], msg2.data[2], msg2.data[3],
                msg2.data[4], msg2.data[5], msg2.data[6], msg2.data[7]
            );
            break;
        }

        
        
        msg.ID = 5;
        msg.length = 4;
        msg.data[0] = 'y';
        msg.data[1] = 'a';
        msg.data[2] = 'y';
        msg.data[3] = '!';

        CAN_send(msg);        
        
        //delay(1000);
        
    }
	*/
}





void can_test(void){

    mcp2515_reset();
    mcp2515_bit_modify(MCP_CANCTRL, MCP_CANCTRL__MODE_LOOPBACK);
    
    printf("\nMCP_CANCTRL: %x\n", mcp2515_read(MCP_CANCTRL));
    
    can_msg_t msg1;
    can_msg_t msg2;
    msg1.ID = 9;
    msg1.length = 5;
    for(int i = 0; i < 8; i++){
        msg1.data[i] = i*i;
    }

    while(1){
        CAN_send(msg1);
        msg2 = CAN_recv();
        printf(("msg1: can_msg_t(id:%d, len:%d, data:(%d, %d, %d, %d, %d, %d, %d, %d))\n"),
        msg1.ID,
        msg1.length,
        msg1.data[0], msg1.data[1], msg1.data[2], msg1.data[3],
        msg1.data[4], msg1.data[5], msg1.data[6], msg1.data[7]
        );
        printf(("msg2: can_msg_t(id:%d, len:%d, data:(%d, %d, %d, %d, %d, %d, %d, %d))\n\n"),
        msg2.ID,
        msg2.length,
        msg2.data[0], msg2.data[1], msg2.data[2], msg2.data[3],
        msg2.data[4], msg2.data[5], msg2.data[6], msg2.data[7]
        );
        msg1.data[0]++;
        
        delay(1000);
    }
}
