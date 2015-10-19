
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../drivers/analog/slider.h"
#include "../drivers/analog/joystick.h"
#include "../../lib/can/can.h"
#include "../drivers/display/frame_buffer.h"
#include "../drivers/display/fonts/font5x7w.h"
#include "../../ArduinoMega2560/can_types.h"
#include "../fifoqueue/fifoqueue.h"
#include "../userprofile/userprofile.h"




typedef struct InputController InputController;
struct InputController {
    uint8_t (*motorInputFcn)(uint8_t sensitivity);
    uint8_t motorSensitivity;
    uint8_t (*servoInputFcn)(void);
    uint8_t (*solenoidInputFcn)(void);
    uint8_t useBluetooth;
};


void sendNewInput(InputController* ic){
    if(!ic->useBluetooth){
        ControlCmd cmd = (ControlCmd){
            .servoPos   = ic->servoInputFcn(),
            .motorSpeed = ic->motorInputFcn(ic->motorSensitivity),
            .solenoid   = ic->solenoidInputFcn(),
        };

        can_msg_t m;
        m.id        = CANID_ControlCmd;
        m.length    = 3;
        memcpy(&m.data, &cmd, m.length);

        can_send(m);
    }
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
    return ((int16_t)(joystick_position().x) * 
        motorSensitivityMultiplier(sensitivity)) / 100;
}
uint8_t JOY_Y_to_motor(uint8_t sensitivity){
    return ((int16_t)(joystick_position().y) * 
        motorSensitivityMultiplier(sensitivity)) / 100;
}
uint8_t SLI_R_to_motor(uint8_t sensitivity){
    return ((-(int16_t)(slider_position().R) + 128) *
        motorSensitivityMultiplier(sensitivity)) / 128;
}
uint8_t SLI_L_to_motor(uint8_t sensitivity){
    return ((-(int16_t)(slider_position().L) + 128) *
        motorSensitivityMultiplier(sensitivity)) / 128;
}

uint8_t JOY_X_to_servo(void){
    return (90 - ((int16_t)(joystick_position().x) * 90) / 100);
}
uint8_t JOY_Y_to_servo(void){
    return (90 - ((int16_t)(joystick_position().y) * 90) / 100);
}
uint8_t SLI_R_to_servo(void){
    return (180 - ((int16_t)(slider_position().R) * 7) / 10);
}
uint8_t SLI_L_to_servo(void){
    return (180 - ((int16_t)(slider_position().L) * 7) / 10);
}

uint8_t JOY_BTN_to_solenoid(void){
    return joystick_button();
}
uint8_t JOY_UP_to_solenoid(void){
    return joystick_position().y > 50;
}
uint8_t SLI_BTN_R_to_solenoid(void){
    return slider_right_button();
}




static uint16_t     lifeTime;


void disableTimerInterrupt(__attribute__((unused)) uint8_t* v){
    TIMSK  &= ~(1 << OCIE1A);   // Disable interrupt on OCR1A match
}    


void game_pong(void){

    UserProfile p = getCurrentUserProfile();

    
    InputController ic = {
        .motorInputFcn =
            p.game_pong.motorInputType == CONTROL_JOY_X ?   JOY_X_to_motor  :
            p.game_pong.motorInputType == CONTROL_JOY_Y ?   JOY_Y_to_motor  :
            p.game_pong.motorInputType == CONTROL_SLI_R ?   SLI_R_to_motor  :
            p.game_pong.motorInputType == CONTROL_SLI_L ?   SLI_L_to_motor  :
                                                            JOY_X_to_motor  ,

        .motorSensitivity = p.game_pong.motorSensitivity ?: 3,

        .servoInputFcn =
            p.game_pong.servoInputType == CONTROL_JOY_X ?   JOY_X_to_servo  :
            p.game_pong.servoInputType == CONTROL_JOY_Y ?   JOY_Y_to_servo  :
            p.game_pong.servoInputType == CONTROL_SLI_R ?   SLI_R_to_servo  :
            p.game_pong.servoInputType == CONTROL_SLI_L ?   SLI_L_to_servo  :
                                                            SLI_R_to_servo  ,

        .solenoidInputFcn = 
            p.game_pong.solenoidInputType == CONTROL_JOY_BTN    ?   JOY_BTN_to_solenoid     :
            p.game_pong.solenoidInputType == CONTROL_JOY_UP     ?   JOY_UP_to_solenoid      :
            p.game_pong.solenoidInputType == CONTROL_SLI_BTN_R  ?   SLI_BTN_R_to_solenoid   :
                                                                    JOY_UP_to_solenoid      ,

        .useBluetooth = p.game_pong.useBluetooth,
    };
    
    

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
        fbuf_clear();
        fbuf_set_font(font5x7w());
        fbuf_set_font_spacing(1, 1);
        fbuf_printf(
            "PONG\n"
            " You vs Gravity"
        );
        fbuf_set_cursor(0, 7);
        fbuf_printf("[Quit]");
        fbuf_render();
    }
            
            
    renderBackground();
    
    
    while(!quit){        
        if(slider_left_button()){
            printf("quitting..\n");
            quit = 1;
        }
        
        sendNewInput(&ic);
        
        if(prevLifeTime != lifeTime){
            prevLifeTime = lifeTime;
            fbuf_set_cursor_to_pixel(0, 28);
            fbuf_printf("%5d Seconds", lifeTime);
            fbuf_render();
        }        
        
        
        recvMsg = can_recv();
        switch(recvMsg.id){
            case CANID_GameOver:
                fbuf_set_cursor_to_pixel(20, 20);
                fbuf_printf(
                    "Game over!\n"
                );
                fbuf_set_cursor(0, 6);
                fbuf_printf(
                    "Play again?\n"
                    "[No]       [Yes]"
                );
                fbuf_render();
                
                if(lifeTime > p.game_pong.bestScore){
                    p.game_pong.bestScore = lifeTime;
                    writeCurrentUserProfile(&p);
                }                    
                
                while(1){
                    if(slider_left_button()){
                        quit = 1;
                        break;
                    }
                    if(slider_right_button()){
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

