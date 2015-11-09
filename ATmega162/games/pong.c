
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "../drivers/analog/slider.h"
#include "../drivers/analog/joystick.h"
#include "../../lib/can/can.h"
#include "../../lib/can/can_types.h"
#include "../drivers/display/frame_buffer.h"
#include "../drivers/display/fonts/font5x7w.h"
#include "../drivers/display/oled.h"
#include "../fifoqueue/fifoqueue.h"
#include "../userprofile/userprofile.h"
#include "../../lib/macros.h"




typedef struct InputController InputController;
struct InputController {
    MotorControlType    motorControlType;
    uint8_t             motorSensitivity;
    int8_t              (*motorSpeedInputFcn)(uint8_t sensitivity);
    uint8_t             (*motorPositionInputFcn)(void);

    uint8_t             servoReversed;
    int8_t              (*servoInputFcn)(uint8_t reversed);

    uint8_t             (*solenoidInputFcn)(void);

    uint8_t             useBluetooth;
};


void sendNewInput(InputController* ic){
    if(!ic->useBluetooth){
        Pong_ControlCmd cmd;
        memset(&cmd, 0, sizeof(Pong_ControlCmd));

        cmd.motor.controlType  = ic->motorControlType;
        switch(ic->motorControlType){
        case MC_Speed:
            cmd.motor.speed = ic->motorSpeedInputFcn(ic->motorSensitivity);
            break;
        case MC_Position:
            cmd.motor.position = ic->motorPositionInputFcn();
            break;
        }

        cmd.servo              = ic->servoInputFcn(ic->servoReversed);
        cmd.solenoid           = ic->solenoidInputFcn();

        can_send(Pong_ControlCmd, cmd);
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


int8_t JOY_X_to_motor(uint8_t sensitivity){
    return ((int16_t)(joystick_position().x) * 
        motorSensitivityMultiplier(sensitivity)) / 100;
}
int8_t JOY_Y_to_motor(uint8_t sensitivity){
    return ((int16_t)(joystick_position().y) * 
        motorSensitivityMultiplier(sensitivity)) / 100;
}
uint8_t SLI_R_to_motor(void){
    return slider_position().R;
}
uint8_t SLI_L_to_motor(void){
    return slider_position().L;
}

int8_t JOY_X_to_servo(uint8_t reversed){
    int8_t v = joystick_position().x;
    v = (v == -128) ? -127 : v;
    return reversed ? v*-1 : v;
}
int8_t JOY_Y_to_servo(uint8_t reversed){
    int8_t v = joystick_position().y;
    v = (v == -128) ? -127 : v;
    return reversed ? v*-1 : v;
}
int8_t SLI_R_to_servo(uint8_t reversed){
    return reversed ? 
        128 - slider_position().R :
        slider_position().R - 128;
}
int8_t SLI_L_to_servo(uint8_t reversed){
    return reversed ? 
        128 - slider_position().L :
        slider_position().L - 128;
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





void game_pong(void){

    UserProfile p = getCurrentUserProfile();

    
    InputController ic = {
        .motorControlType = 
            p.game_pong.motorInputType == CONTROL_JOY_X ?   MC_Speed        :
            p.game_pong.motorInputType == CONTROL_JOY_Y ?   MC_Speed        :
            p.game_pong.motorInputType == CONTROL_SLI_R ?   MC_Position     :
            p.game_pong.motorInputType == CONTROL_SLI_L ?   MC_Position     :
                                                            MC_Speed        ,

        .motorSensitivity = p.game_pong.motorSensitivity ?: 3,

        .motorSpeedInputFcn =
            p.game_pong.motorInputType == CONTROL_JOY_X ?   JOY_X_to_motor  :
            p.game_pong.motorInputType == CONTROL_JOY_Y ?   JOY_Y_to_motor  :
                                                            NULL            ,

        .motorPositionInputFcn =                                                   
            p.game_pong.motorInputType == CONTROL_SLI_R ?   SLI_R_to_motor  :
            p.game_pong.motorInputType == CONTROL_SLI_L ?   SLI_L_to_motor  :
                                                            NULL            ,

        .servoReversed = p.game_pong.servoReversed,

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


    // Disable timer interrupts on scope exit
    scope_exit {
        TIMSK  &= ~(1 << OCIE1A);
    }
    
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
    
    
    while(1){   
        if(slider_left_button()){
            return;
        }
        
        sendNewInput(&ic);
        
        if(prevLifeTime != lifeTime){
            prevLifeTime = lifeTime;
            fbuf_set_cursor_to_pixel(0, 28);
            fbuf_printf("%5d Seconds", lifeTime);
            fbuf_render();
        }        
        
        
        can_receive(nonblock,
            Pong_GameOver, __attribute__((unused)) gameOver, {
                fbuf_set_cursor_to_pixel(20, 20);
                fbuf_printf(
                    "Game over!\n"
                );
                fbuf_set_cursor_to_pixel(0, DISP_HEIGHT-16);
                fbuf_printf("Play again?");
                fbuf_set_cursor_to_pixel(0, DISP_HEIGHT-8);
                fbuf_printf("[No]");
                fbuf_set_cursor_to_pixel(
                    DISP_WIDTH  - fbuf_get_current_font_config().width * 5,
                    DISP_HEIGHT - 8
                );
                fbuf_printf("[Yes]");
                fbuf_render();
                
                if(lifeTime > p.game_pong.bestScore){
                    p.game_pong.bestScore = lifeTime;
                    writeCurrentUserProfile(&p);
                }
                
                while(1){
                    if(slider_left_button()){
                        return;
                    }
                    if(slider_right_button()){
                        renderBackground();
                        lifeTime = 0;
                        break;
                    }
                }
            }
        );

        _delay_ms(10);
    }
}

ISR(TIMER1_COMPA_vect){
    lifeTime++;
}

