
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lib/can/can.h"
#include "../lib/can/can_types.h"
#include "../lib/can/can_test.h"
#include "../lib/uart/uart.h"

#include "drivers/core/adc/adc.h"
#include "drivers/core/pwm/pwm.h"

#include "drivers/app/ir_led.h"
#include "drivers/app/motor.h"
#include "drivers/app/solenoid.h"
#include "drivers/app/servo.h"

#include "control/timer.h"
#include "control/pid.h"




uint16_t findGameBoardEncoderMax(void){
    // Drive left & reset
    motor_velocity(-0.5);
    _delay_ms(1500);
    motor_velocity(0.0);
    _delay_ms(100);
    motor_encoderReset();

    // Drive right & return max value
    motor_velocity(0.5);
    _delay_ms(1500);
    motor_velocity(0.0);
    _delay_ms(100);
    return motor_encoderRead();
}


int main(void){
    uart_init();
    can_init();
    adc_init();
    servo_init(0.9, 2.1);
    motor_init();
    ir_obstructThreshold(10);
    solenoid_init();

    printf("started\n");

    float gameBoardEncoderMax = findGameBoardEncoderMax();
    float gameBoardPositionMultiplier = gameBoardEncoderMax / 255.0;

    TCCR1B |= (3 << CS10);
    PID_controller* motorPID = pid_new(1, 0.7, 0, (Timer){&TCNT1, 64});


    while(1){

        
        static uint8_t prev_ir_obstruction = 0;
        uint8_t ir_obstruction = ir_obstructed();
        if(ir_obstruction != prev_ir_obstruction){
            //printf("IR %sobstructed\n", ir_obstruction ? "" : "un");
            //can_send((can_msg_t){
            //    .id = CANID_GameOver
            //});
        }
        prev_ir_obstruction = ir_obstruction;
        

        
        can_receive(nonblock,
            Pong_ControlCmd, control, {
                // Servo
                servo_write(control.servo);
            
                // Solenoid
                solenoid(control.solenoid);

                // Motor
                switch(control.motor.controlType){
                case MC_Speed:
                    //printf("vel: %f\n", control.motor.speed/127.0);
                    motor_velocity(control.motor.speed/127.0);

                    break;
                case MC_Position:
                
                    //printf("pos: %d\n", control.motor.position);
                
                    pid_reference(motorPID, (float)control.motor.position * gameBoardPositionMultiplier);
                    float y = motor_encoderRead();
                    float u = pid_actuate(motorPID, y)/gameBoardEncoderMax;
                    motor_velocity(u);
                    //printf("u:%3.3f  \ty:%3.3f\n", u, y);

                    break;
                default:
                    break;
                }
            }
        );

        
        _delay_ms(10);
    }
}


