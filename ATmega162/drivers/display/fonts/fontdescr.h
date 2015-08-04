#pragma once

#include <stdint.h>

typedef struct {
    void*   addr;
    uint8_t width;
    uint8_t height;
    uint8_t start_offset;

} FontDescr;