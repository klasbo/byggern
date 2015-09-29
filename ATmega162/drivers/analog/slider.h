
#pragma once

#include <inttypes.h>

typedef struct {
    uint8_t L;
    uint8_t R;
} SLI_pos_t ;

void slider_init(void);

SLI_pos_t slider_position(void);

int slider_right_button(void);
int slider_left_button(void);


