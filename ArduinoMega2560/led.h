#pragma once

#include <Arduino.h>

#define analogLed A0
#define threshold 50


void LED_init(void);
uint8_t LED_read(void);
uint8_t LED_score_count();