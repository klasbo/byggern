#pragma once

typedef struct Timer Timer;
struct Timer {
    volatile uint16_t*  source;
    uint16_t            prescaler;
};