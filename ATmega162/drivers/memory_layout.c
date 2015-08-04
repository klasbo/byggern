
#include <avr/io.h>

void __attribute__ ((constructor)) memory_layout_init(void){
    MCUCR |= (1 << SRE);
    SFIOR |= (1 << XMM2);
}
