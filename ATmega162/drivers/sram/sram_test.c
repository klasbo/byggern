
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "../memory_layout.h"




void sram_test(void){
    unsigned int werrors = 0;
    unsigned int rerrors = 0;
    unsigned char testvalue;
    printf_P(PSTR("Starting SRAM test...\n"));
    
    uint16_t seed = TCNT3;

    srandom(seed);
    for (int i = 0; i < ext_ram_size; i++) {
        testvalue = random();
        ext_ram[i] = testvalue;
        if (ext_ram[i] != testvalue) {
            //printf_P(PSTR("SRAM error (write phase): ext_ram[%4d] = %02X (should be %02X)\n"), i, ext_ram[i], testvalue);
            werrors++;
        }
    }
    srandom(seed);
    for (int i = 0; i < ext_ram_size; i++) {
        testvalue = random();
        if (ext_ram[i] != testvalue) {
            //printf_P(PSTR("SRAM error (read phase): ext_ram[%4d] = %02X (should be %02X)\n"), i, ext_ram[i], testvalue);
            rerrors++;
        }
    }
    printf_P(PSTR("SRAM test completed with:\n  Write phase errors: %4d\n  Read phase errors:  %4d\n"), werrors, rerrors);
}
