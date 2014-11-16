#pragma once

#include <Servo.h>
#include <stdint.h>


void BT_run(Servo* s);
void BT_enable_interrupts();
void BT_init(Servo* servo, double sampletime);
void BT_stop(void);

