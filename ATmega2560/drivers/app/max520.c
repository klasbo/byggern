
#include "max520.h"

#include <avr/interrupt.h>

#include "../core/twi/TWI_Master.h"


#define MAX520_TWI_ADDR_BASE 0b01010000
static uint8_t twi_addr = 0b000;

void max520_init(uint8_t max520_twi_addr){
    TWI_Master_Initialise();
    sei();  // TWI driver uses interrupts
    twi_addr = max520_twi_addr;
}

void max520_write(uint8_t ch, uint8_t val){
    uint8_t msg[3] = {
        MAX520_TWI_ADDR_BASE | ((twi_addr & 0x07) << 1),
        ch & 0x07,
        val
    };
    TWI_Start_Transceiver_With_Data(msg, 3);
}