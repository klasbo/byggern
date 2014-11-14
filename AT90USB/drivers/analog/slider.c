
#include <avr/io.h>

#include "slider.h"
//#include "touch/touch.h"

void __attribute__ ((constructor)) slider_init(void){
    //touch_init(30, 30, 30, 30);
}    

SLI_pos_t SLI_get_slider_position(void){
    //unsigned char ls, rs, lb, rb;
    //touch_measure(&ls, &rs, &lb, &rb);
    return (SLI_pos_t){
        .L = 0,//ls,
        .R = 0//rs
    };
}

int SLI_get_right_button(void){
    //unsigned char ls, rs, lb, rb;
    //touch_measure(&ls, &rs, &lb, &rb);
    //return rb;
    return 0;
}    

int SLI_get_left_button(void){
    //unsigned char ls, rs, lb, rb;
    //touch_measure(&ls, &rs, &lb, &rb);
    //return lb;
    return 0;
}    
