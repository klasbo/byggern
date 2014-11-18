#pragma once

#include <Arduino.h>

#include "pin_config.h"

#define threshold 50


void IR_init(void);
uint8_t IR_obstructed(void);