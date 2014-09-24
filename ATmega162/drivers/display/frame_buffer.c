

#include <stdint.h>
#include <stdio.h>

#include "font.h"
#include "oled.h"
#include "frame_buffer.h"
#include "../memory_layout.h"

#define frame ( (uint8_t(*)[COLUMNS]) (ext_ram) )

uint8_t reverse_bits(uint8_t x){
    x = ((x & 0x55) << 1) | ((x & 0xaa) >> 1);
    x = ((x & 0x33) << 2) | ((x & 0xcc) >> 2);
    x = ((x & 0x0f) << 4) | ((x & 0xf0) >> 4);
    return x;
}

void draw_byte(uint8_t b, uint8_t column, uint8_t page){
    frame[page][column] = b;
}    

//TODO: rename!
void draw_upper_n(uint8_t b, uint8_t where_x, uint8_t where_y, uint8_t n){
    uint8_t page      = where_y/8;
    uint8_t offset    = where_y%8;

    uint8_t mask1 = ((uint8_t)(0xff >> (8 - offset))  |  (uint8_t)(0xff << (n + offset)));
    uint8_t val1  = (uint8_t)(b << offset);

    frame[page][where_x] = val1 ^ ((val1 ^ frame[page][where_x]) & mask1);

    if(page + 1 < PAGES  &&  n + offset > 8){
        uint8_t mask2 = (uint8_t)(0xff << (offset - (8-n)));
        uint8_t val2  = (uint8_t)(b >> (8 - offset));

        frame[page+1][where_x]  = val2 ^ ((val2 ^ frame[page+1][where_x]) & mask2);
    }
}
    
// TODO: currently: starts drawing n bits lower from where_y. Change to variant of draw_upper_n with shifted value?
void draw_lower_n(uint8_t b, uint8_t where_x, uint8_t where_y, uint8_t n){
    uint8_t page      = where_y/8;
    uint8_t offset    = where_y%8;

    if(offset < n){
        uint8_t mask1 = (uint8_t)(0xff >> (n - offset));
        uint8_t val1  = (uint8_t)(b << offset);
            
        frame[page][where_x] = val1 ^ ((val1 ^ frame[page][where_x]) & mask1);
    }

    if(page + 1 < PAGES  &&  offset > 0){
        uint8_t mask2 = (uint8_t)(0xff >> (8 + n - offset)) | (uint8_t)(0xff << (offset));
        uint8_t val2  = (uint8_t)(b >> (8 - offset));

        frame[page+1][where_x]  = val2 ^ ((val2 ^ frame[page+1][where_x]) & mask2);
    }
}

void render(void){
    for(int i = 0; i < PAGES; i++){
        OLED_go_to_line(i);
        int j;
        for(j = 0; j < COLUMNS; j++){
            /*
            if(frame[i][j] != 0){
                printf("frame[%d][%d] = %d\n", i, j, frame[i][j]);
            }
            */
            *ext_oled_data = (char)frame[i][j];
        }
    }
}


