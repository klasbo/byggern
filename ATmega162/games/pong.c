#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../drivers/analog/slider.h"
#include "../drivers/analog/joystick.h"
#include "../drivers/communication/can/can.h"
#include "../drivers/display/frame_buffer.h"
#include "../drivers/display/fonts/font8x8.h"
#include "../../ArduinoMega2560/can_types.h"
#include "../fifoqueue/fifoqueue.h"
#include "../userprofile/userprofile.h"




typedef struct InputController InputController;
struct InputController {
    uint8_t (*motorInputFcn)(uint8_t sensitivity);
    uint8_t motorSensitivity;
    uint8_t (*servoInputFcn)(void);
    uint8_t (*solenoidInputFcn)(void);
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
        sensitivity == 1    ?   15      :
        sensitivity == 2    ?   30      :
        sensitivity == 3    ?   50      :
        sensitivity == 4    ?   70      :
        sensitivity == 5    ?   100     :
                                50      ;
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
    return JOY_get_button();
}
uint8_t JOY_UP_to_solenoid(void){
    return JOY_get_position().y > 50;
}
uint8_t SLI_BTN_R_to_solenoid(void){
    return SLI_get_right_button();
}




static uint16_t     lifeTime;


void disableTimerInterrupt(__attribute__((unused)) uint8_t* v){
    TIMSK  &= ~(1 << OCIE1A);   // Disable interrupt on OCR1A match
}    


void game_pong(void){

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
    TCCR1B  |=  (1 << WGM12);   // Clear TCNT1 on compare match
    TCCR1B  |=  (1 << CS32);    // Enable timer, 256x prescaler
    TIMSK   |=  (1 << OCIE1A);  // Enable interrupt on OCR1A match
    OCR1A   =   (F_CPU/256);
    sei();    
    // scope(exit) hacking
    __attribute__((cleanup(disableTimerInterrupt))) uint8_t disableTimerInterruptVar;

    
    can_msg_t       recvMsg;
    uint8_t         quit                = 0;
    lifeTime                            = 0;
    uint16_t        prevLifeTime        = -1;


    void renderBackground(void){
        frame_buffer_clear();
        frame_buffer_set_font(font8x8, FONT8x8_WIDTH, FONT8x8_HEIGHT, FONT8x8_START_OFFSET);
        frame_buffer_printf(
            "PONG\n"
            " You vs Gravity"
        );
        frame_buffer_set_cursor(0, 7*FONT8x8_HEIGHT);
        frame_buffer_printf("[Quit]");
        frame_buffer_render();
    }
            
            
    renderBackground();
    
    
    while(!quit){        
        if(SLI_get_left_button()){
            printf("quitting..\n");
            quit = 1;
        }
        
        CAN_send(genControlCmd(&ic));
        
        if(prevLifeTime != lifeTime){
            prevLifeTime = lifeTime;
            frame_buffer_set_cursor(0, 28);
            frame_buffer_printf("%5d Seconds", lifeTime);
            frame_buffer_render();
        }        
        
        
        recvMsg = CAN_recv();
        switch(recvMsg.ID){
            case CANID_GameOver:
                frame_buffer_set_cursor(20, 20);
                frame_buffer_printf(
                    "Game over!\n"
                );
                frame_buffer_set_cursor(0, 6*FONT8x8_HEIGHT);
                frame_buffer_printf(
                    "Play again?\n"
                    "[No]       [Yes]"
                );
                frame_buffer_render();
                
                if(lifeTime > p.game_pong.bestScore){
                    p.game_pong.bestScore = lifeTime;
                    writeCurrentUserProfile(&p);
                }                    
                
                while(1){
                    if(SLI_get_left_button()){
                        quit = 1;
                        break;
                    }
                    if(SLI_get_right_button()){
                        renderBackground();
                        lifeTime = 0;
                        break;
                    }
                }
                break;
            default:
                break;
        }
    }
    
    return;
}

ISR(TIMER1_COMPA_vect){
    lifeTime++;
}

