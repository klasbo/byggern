
#include <avr/io.h>
#include "sram/sram.h"

void memory_layout_init(void){
    MCUCR |= (1 << SRE);
    SFIOR |= (1 << XMM2);
    sram_clear();
}
