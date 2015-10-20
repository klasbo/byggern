
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





static inline float constrain_int8_t(int8_t val, float lower, float upper){
    return (((float)val + 128.0) * (upper-lower) / 256.0  +  lower);
}


int main(void){
    uart_init();
    can_init();
    adc_init();
    pwm_period(20.0f);
    pwm_width(1.5f);
    motor_init();
    ir_obstruct_threshold(10);

    printf("started\n");

    while(1){

        
        static uint8_t prev_ir_obstruction = 0;
        uint8_t ir_obstruction = ir_obstructed();
        if(ir_obstruction != prev_ir_obstruction){
            printf("IR %sobstructed\n", ir_obstruction ? "" : "un");
            //can_send((can_msg_t){
            //    .id = CANID_GameOver
            //});
        }
        prev_ir_obstruction = ir_obstruction;
        

        
        can_msg_t msg = can_recv();
        switch(msg.id){
        case CANID_ControlCmd: ;
            CanMsg_ControlCmd control = union_cast(CanMsg_ControlCmd, msg.data);

            // Servo
            pwm_width(constrain_int8_t(control.servo, 0.9, 2.1));
            
            // Solenoid
            control.solenoid ?
                (PORTH |= (1 << PH4)) :
                (PORTH &= ~(1 << PH4));

            // Motor
            switch(control.motor.controlType){
            case MC_Speed:
                motor_direction( (control.motor.speed > 0) ? right : left );
                motor_speed(abs(control.motor.speed));
                break;
            case MC_Position:
                
                // insert reference tracking controller here...
                
                break;
            default:
                break;
            }
            break;

        default:
            break;
        }

        
        _delay_ms(10);
    }
}


