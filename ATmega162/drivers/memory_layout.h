
#pragma once

// Start addresses for:

//   SRAM
extern volatile char* ext_ram;
//   ADC
extern volatile char* ext_adc;
//   OLED Command
extern volatile char* ext_oled_cmd;
//   OLED Data
extern volatile char* ext_oled_data;

#define ext_ram_size 0x800