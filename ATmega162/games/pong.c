#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../drivers/analog/analog_read.h"
#include "../drivers/communication/can/can.h"
#include "../../ArduinoMega2560/can_types.h"
#include "../fifoqueue/fifoqueue.h"

#include "../drivers/analog/slider.h"
#include "../drivers/analog/joystick.h"


#define pollRate_hz 4

/*
usersettings users[MAX_NUM_USERS];
usersettings {
    char username[MAX_USERNAME_LENGTH];
    struct {
        uint32_t    highscore;

        uint8_t     inputMap[3];

        uint8_t     (*inputFcnServo)(void);
        int8_t      (*inputFcnMotorSpeed)(void);
        uint8_t     (*inputFcnSolenoid)(void);
    } game_pong;
    struct {
        
    } game_2048;
}

usersettings* currentUser;
*/



can_msg_t genControlCmd(void){
    ControlCmd cmd = (ControlCmd){
        .servoPos   = (180 - ((int16_t)(SLI_get_slider_position().R) * 7) / 10),
        .motorSpeed = JOY_get_position().x, 
        .solenoid   = JOY_get_position().y < 50,
    };

    can_msg_t m;
    m.ID        = CANID_ControlCmd;
    m.length    = 3;
    memcpy(m.data, &cmd, m.length);

    return m;
}


typedef struct pollInputEvent pollInputEvent;
struct pollInputEvent {
    uint8_t unused;
};

#define eventType_pollInput 99
#define eventType_canMsg 98

static fifoqueue_t* q;

void game_pong(void){
    q = new_fifoqueue();

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
        CAN_send(genControlCmd());
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

