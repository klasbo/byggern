
#include "../memory_layout.h"
#include "sram.h"

void __attribute__ ((constructor)) sram_init(void){
    extern void memory_layout_init(void);
    memory_layout_init();
    sram_clear();
}

void sram_clear(){
    for (int i = 0; i < ext_ram_size; i++){
        ext_ram[i] = 0;
    }
}