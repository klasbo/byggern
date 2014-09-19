
#include "config.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#include "drivers/uart.h"
#include "drivers/adc.h"
#include "drivers/joystick.h"
#include "drivers/slider.h"
#include "drivers/oled.h"
#include "drivers/memory_layout.h"

void SRAM_test(void);

int main(void){
    
    fdevopen(&UART_transmit, &UART_receive);
    
    SRAM_test();
    
    JOY_set_new_center();
    JOY_pos_t joy_pos;
    SLI_pos_t sli_pos;
    
    int c = 0;
    
    while(1){
        
        sli_pos = SLI_get_slider_position();
        printf("Slider position: (Left:%d, Right:%d)\n", sli_pos.L, sli_pos.R);
        printf("Slider buttons:  (%s, %s)\n", (SLI_get_left_button() ? "LEFT" : "Left"), (SLI_get_right_button() ? "RIGHT" : "Right") );
        joy_pos = JOY_get_position();
        printf("pos_x: %4d  |  pos_y: %4d\n", joy_pos.x, joy_pos.y);
        printf("Joy dirn: %d\n", JOY_get_direction());
        
                
        printf("\n\n");
        OLED_printf(
            "hello world\n"
            " iteration %d\n", c++
        );
                
        _delay_ms(1000);
    }
}


void SRAM_test(void){
    unsigned int i, werrors, rerrors;
    werrors = 0;
    rerrors = 0;
    unsigned char testvalue;
    printf("Starting SRAM test...\r\n");
    for (i = 0; i < 0x800; i++) {
        testvalue = ~(i % 256);
        ext_ram[i] = testvalue;
        if (ext_ram[i] != testvalue) {
            printf("SRAM error (write phase): ext_ram[%4d] = %02X (should be %02X)\r\n", i, ext_ram[i], testvalue);
            werrors++;
        }
    }
    for (i = 0; i < 0x800; i++) {
        testvalue = ~(i % 256);
        if (ext_ram[i] != testvalue) {
            printf("SRAM error (read phase): ext_ram[%4d] = %02X (should be %02X)\r\n", i, ext_ram[i], testvalue);
            rerrors++;
        }
    }
    printf("SRAM test completed with %d errors in write phase and %d errors in read phase\r\n", werrors, rerrors);
}

