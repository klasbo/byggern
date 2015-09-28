
#pragma once

#include <inttypes.h>

typedef struct {
    uint8_t L;
    uint8_t R;
} SLI_pos_t ;

void slider_init(void);

SLI_pos_t SLI_get_slider_position(void);

int SLI_get_right_button(void);
int SLI_get_left_button(void);


