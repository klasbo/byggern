#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>


#include "../drivers/analog/slider.h"
#include "../drivers/analog/joystick.h"
#include "../drivers/communication/can/can.h"
#include "../../ArduinoMega2560/can_types.h"
#include "../fifoqueue/fifoqueue.h"
#include "../userprofile/userprofile.h"



#define pollRate_hz 4

#define eventType_pollInput 99
#define eventType_canMsg    98

typedef struct InputController InputController;
struct InputController {
    uint8_t (*motorInputFcn)(uint8_t sensitivity);
    uint8_t motorSensitivity;
    uint8_t (*servoInputFcn)(void);
    uint8_t (*solenoidInputFcn)(void);
};

typedef struct pollInputEvent pollInputEvent;
struct pollInputEvent {
    uint8_t unused;
};




can_msg_t genControlCmd(InputController* ic){
    ControlCmd cmd = (ControlCmd){
        .servoPos   = ic->servoInputFcn(),
        .motorSpeed = ic->motorInputFcn(ic->motorSensitivity),
        .solenoid   = ic->solenoidInputFcn(),
    };

    can_msg_t m;
    m.ID        = CANID_ControlCmd;
    m.length    = 3;
    memcpy(m.data, &cmd, m.length);

    return m;
}



int16_t motorSensitivityMultiplier(uint8_t sensitivity){
    return
        sensitivity == 1    ?   10      :
        sensitivity == 2    ?   25      :
        sensitivity == 3    ?   40      :
        sensitivity == 4    ?   70      :
        sensitivity == 5    ?   100     :
                                40      ;
}


uint8_t JOY_X_to_motor(uint8_t sensitivity){
    return ((int16_t)(JOY_get_position().x) * 
        motorSensitivityMultiplier(sensitivity)) / 100;
}
uint8_t JOY_Y_to_motor(uint8_t sensitivity){
    return ((int16_t)(JOY_get_position().y) * 
        motorSensitivityMultiplier(sensitivity)) / 100;
}
uint8_t SLI_R_to_motor(uint8_t sensitivity){
    return ((-(int16_t)(SLI_get_slider_position().R) + 128) *
        motorSensitivityMultiplier(sensitivity)) / 128;
}
uint8_t SLI_L_to_motor(uint8_t sensitivity){
    return ((-(int16_t)(SLI_get_slider_position().L) + 128) *
        motorSensitivityMultiplier(sensitivity)) / 128;
}

uint8_t JOY_X_to_servo(void){
    return (90 - ((int16_t)(JOY_get_position().x) * 90) / 100);
}
uint8_t JOY_Y_to_servo(void){
    return (90 - ((int16_t)(JOY_get_position().y) * 90) / 100);
}
uint8_t SLI_R_to_servo(void){
    return (180 - ((int16_t)(SLI_get_slider_position().R) * 7) / 10);
}
uint8_t SLI_L_to_servo(void){
    return (180 - ((int16_t)(SLI_get_slider_position().L) * 7) / 10);
}

uint8_t JOY_BTN_to_solenoid(void){
    return !JOY_get_button();
}
uint8_t JOY_UP_to_solenoid(void){
    return JOY_get_position().y < 50;
}
uint8_t SLI_BTN_R_to_solenoid(void){
    return !SLI_get_right_button();
}

static fifoqueue_t* q;

void game_pong(void){
    
    q = new_fifoqueue();

    InputController ic;
    UserProfile p = getCurrentUserProfile();

    ic.motorInputFcn =
        p.game_pong.motorInputType == CONTROL_JOY_X ?   JOY_X_to_motor  :
        p.game_pong.motorInputType == CONTROL_JOY_Y ?   JOY_Y_to_motor  :
        p.game_pong.motorInputType == CONTROL_SLI_R ?   SLI_R_to_motor  :
        p.game_pong.motorInputType == CONTROL_SLI_L ?   SLI_L_to_motor  :
                                                        JOY_X_to_motor  ;

    ic.motorSensitivity = p.game_pong.motorSensitivity ?: 3;

    ic.servoInputFcn =
        p.game_pong.servoInputType == CONTROL_JOY_X ?   JOY_X_to_servo  :
        p.game_pong.servoInputType == CONTROL_JOY_Y ?   JOY_Y_to_servo  :
        p.game_pong.servoInputType == CONTROL_SLI_R ?   SLI_R_to_servo  :
        p.game_pong.servoInputType == CONTROL_SLI_L ?   SLI_L_to_servo  :
                                                        SLI_R_to_servo  ;

    ic.solenoidInputFcn = 
        p.game_pong.solenoidInputType == CONTROL_JOY_BTN    ?   JOY_BTN_to_solenoid     :
        p.game_pong.solenoidInputType == CONTROL_JOY_UP     ?   JOY_UP_to_solenoid      :
        p.game_pong.solenoidInputType == CONTROL_SLI_BTN_R  ?   SLI_BTN_R_to_solenoid   :
                                                                JOY_UP_to_solenoid      ;
    
    

    cli();
    // attach timer interrupt
    //TCCR1A |= (1 << COM1A1);   // Clear OC1A on compare match
    TCCR1B  |=  (1 << WGM12);   // Clear TCNT1 on compare match
    TCCR1B  |=  (1 << CS32);    // Enable timer, 256x prescaler
    TIMSK   |=  (1 << OCIE1A);  // Enable interrupt on OCR1A match
    
    OCR1A = (F_CPU/256)/pollRate_hz;
    sei();

    
    printf("OCR1A: %d\n", OCR1A);
    printf("TCNT1: %d\n", TCNT1);

    can_msg_t       recvMsg;
    pollInputEvent  pollInputEvent;
    uint8_t         quit = 0;

    
    while(!quit){
        if(SLI_get_left_button()){
            printf("quitting..\n");
            quit = 1;
        }
        // send ControlCmd
        CAN_send(genControlCmd(&ic));
        _delay_ms(20);
    }
    

    /*
    while(!quit){
        //print_fifoqueue_t(q);

        cli();
        uint8_t f = front_type(q);
        sei();

        switch(f){
        case eventType_pollInput:

            cli();
            dequeue(q, &pollInputEvent);
            sei();

            //printf("polling..\n");
            // check for quit input
            if(SLI_get_left_button()){
                printf("quitting..\n");
                quit = 1;
            }
            // send ControlCmd
            CAN_send(genControlCmd());
            break;

        case eventType_canMsg:

            cli();
            dequeue(q, &recvMsg);
            sei();

            printf("new can msg\n");
            switch(recvMsg.ID){
            case CANID_IRLED:
                break;
                
            }
            break;
        case 0: break;
        default:
            printf("received WAT: %d\n", f);
            break;
        }
    }
    */
    
    
    TIMSK  &= ~(1 << OCIE1A);   // Disable interrupt on OCR1A match
    delete_fifoqueue(&q);
    
    // printf("TCNT1: %d\n", TCNT1);

    return;
}

ISR(TIMER1_COMPA_vect){
    //printf("a\n");
    enqueue(q, eventType_pollInput, 0, 0); //&(pollInputEvent){0}, sizeof(pollInputEvent));
}

