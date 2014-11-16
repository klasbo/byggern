#pragma once

#include <Arduino.h>

#define analogLed A0
#define threshold 50


void IR_init(void);
uint8_t IR_obstructed(void);