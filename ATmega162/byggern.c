#define F_CPU 4195200

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#include "drivers/uart.h"
#include "drivers/adc.h"
#include "drivers/joystick.h"
#include "drivers/slider.h"

void SRAM_test(void);

int main(void)
{
    /*test1
    DDRB = 0x1;
    while(1)
    {
        
        PORTB |= (1<<PB0); 
        _delay_ms(2000);
        PORTB &= ~(1<<PB0);
        _delay_ms(2000);
        
    }
    */
    
    /*    test2
    UART_init(31); //lol magic (FOSC/16/BAUD-1)
    fdevopen(&UART_transmit, &UART_receive);


    int i = 0;
    int j;

    while(1){
        // UART_transmit(UART_receive());

        
        scanf("%d", &j);
        i += j;
        printf("i = %d\n", i);
        
    }
    */
    
    
    UART_init(31); //lol magic (FOSC/16/BAUD-1)
    fdevopen(&UART_transmit, &UART_receive);
    
    MCUCR |= (1 << XMM2) | (1 << SRE);
    
    DDRB = ~0x3;
    SRAM_test();
    JOY_setNewCenter();
    JOY_pos_t pos;
    SLI_pos_t s;
    
    while(1){
        s = SLI_getSliderPosition();
        printf("Slider: (Left:%d, Right:%d)\n", s.L, s.R);
        printf("PINB %d\n", PINB);
        pos = JOY_getPosition();
        printf("pos_x: %d    pos_y: %d\n", pos.x, pos.y);
        printf("Joy dirn: %d\n", JOY_getDirection());
        printf("\n\n");
        _delay_ms(750);
    }
    

    
}


void SRAM_test(void)
{
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

