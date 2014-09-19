
#include <avr/io.h>

void __attribute__ ((constructor)) memory_layout_init(void){
    MCUCR |= (1 << XMM2) | (1 << SRE);
}

volatile char* ext_ram = (char*) 0x1800;

volatile char* ext_adc = (char*) 0x1400;

volatile char* ext_oled_cmd  = (char*) 0x1000;

volatile char* ext_oled_data = (char*) 0x1200;