#pragma once

#include <Servo.h>
#include <stdint.h>

#define solenoidPin 7

void BT_run(Servo* s);
void BT_enable_interrupts();
void BT_init(Servo* servo, double sampletime);
