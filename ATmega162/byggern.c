#define F_CPU 4195200

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#include "drivers/uart.h"
#include "drivers/adc.h"
#include "drivers/joystick.h"
#include "drivers/slider.h"
#include "drivers/oled.h"

void SRAM_test(void);

int main(void){
    
    UART_init(31); //lol magic (FOSC/16/BAUD-1)
    fdevopen(&UART_transmit, &UART_receive);
    
    MCUCR |= (1 << XMM2) | (1 << SRE);  // TODO: Move to memory_layout.h (external memory mask, external memory enable)
    
    DDRB = ~0x3;    // TODO: move to drivers/slider.h (?) (touch buttons)
    
    
    SRAM_test();
    
    JOY_setNewCenter();
    JOY_pos_t joy_pos;
    SLI_pos_t sli_pos;
    
    OLED_init();
    
    int c = 0;
    
    while(1){
        sli_pos = SLI_getSliderPosition();
        printf("Slider: (Left:%d, Right:%d)\n", sli_pos.L, sli_pos.R);
        printf("PINB %d\n", PINB);
        joy_pos = JOY_getPosition();
        printf("pos_x: %d    pos_y: %d\n", joy_pos.x, joy_pos.y);
        printf("Joy dirn: %d\n", JOY_getDirection());
        printf("\n\n");
        
        OLED_printf(
            "hello world\n"
            " iteration %d\n", c++
        );
                
        _delay_ms(1000);
    }
}


void SRAM_test(void){
    volatile char *ext_ram = (char *) 0x1800; // Start address for the SRAM
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

