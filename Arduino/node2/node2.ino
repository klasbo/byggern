#include "test.h"
#include "can.h"
#include "spi.h"
#include "mcp2515.h"

#include <stdio.h>

#include <Arduino.h>


#define ubrr (F_CPU/16/9600 - 1)

void uart_init(void){

    UBRR0L = (unsigned char)(ubrr);

    // USART Control and Status
    UCSR0B =    (1<<RXEN0)      // receive enable
            |   (1<<TXEN0);     // transmit enable

    UCSR0C =    (3<<UCSZ00);    // char size to 8


    fdevopen((int (*)(char, struct __file*))&UART_transmit, (int (*)(struct __file*))&UART_receive);
}

char UART_receive(void){
    while( !(UCSR0A & (1<<RXC0)) ){}    // busy-wait until receive complete

    return UDR0;
}

void UART_transmit(unsigned char c){
    while( !(UCSR0A & (1<<UDRE0)) ){}   // busy-wait until data register empty

    UDR0 = c;
}

void can_test(void);

int main(void){
    extern void init(void);
    init();

    uart_init();
    SPI_init();
    CAN_init();
    //mcp2515_bit_modify(MCP_CANCTRL, MCP_CANCTRL__MODE_LOOPBACK);

    can_msg_t msg;
    //DDRB &= ~(1 << DDB3);
    int iter = 0;
    //can_test();
    while(1){
        printf("iter %4d\n", iter++);
        //mcp2515_read(MCP_CANSTAT);

        /*
        PORTB |= (1 << PB3);
        delay(500);
        PORTB &= ~(1 << PB3);
        delay(300);
        */
        
        can_msg_t msg2 = CAN_recv();
        //if(msg2.ID != 0){
            printf("msg2: can_msg_t(id:%d, len:%d, data:(%d, %d, %d, %d, %d, %d, %d, %d))\n\n",
                msg2.ID,
                msg2.length,
                msg2.data[0], msg2.data[1], msg2.data[2], msg2.data[3],
                msg2.data[4], msg2.data[5], msg2.data[6], msg2.data[7]
            );
        //}
        
        /*
        msg.ID = 5;
        msg.length = 4;
        msg.data[0] = 'y';
        msg.data[1] = 'a';
        msg.data[2] = 'y';
        msg.data[3] = '!';

        CAN_send(msg);
        
        */
        delay(1000);
        
    }

}




void can_test(void){

    mcp2515_reset();
    mcp2515_bit_modify(MCP_CANCTRL, MCP_CANCTRL__MODE_LOOPBACK);
    
    printf("\nMCP_CANCTRL: %x\n", mcp2515_read(MCP_CANCTRL));
    
    can_msg_t msg1;
    can_msg_t msg2;
    msg1.ID = 9;
    msg1.length = 5;
    for(int i = 0; i < 8; i++){
        msg1.data[i] = i*i;
    }

    while(1){
        CAN_send(msg1);
        msg2 = CAN_recv();
        printf(("msg1: can_msg_t(id:%d, len:%d, data:(%d, %d, %d, %d, %d, %d, %d, %d))\n"),
        msg1.ID,
        msg1.length,
        msg1.data[0], msg1.data[1], msg1.data[2], msg1.data[3],
        msg1.data[4], msg1.data[5], msg1.data[6], msg1.data[7]
        );
        printf(("msg2: can_msg_t(id:%d, len:%d, data:(%d, %d, %d, %d, %d, %d, %d, %d))\n\n"),
        msg2.ID,
        msg2.length,
        msg2.data[0], msg2.data[1], msg2.data[2], msg2.data[3],
        msg2.data[4], msg2.data[5], msg2.data[6], msg2.data[7]
        );
        msg1.data[0]++;
        
        delay(1000);
    }
}
