
#include "../memory_layout.h"
#include "sram.h"

void sram_clear(){
    for (int i = 0; i < ext_ram_size; i++){
        ext_ram[i] = 0;
    }
}