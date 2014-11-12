
#include "font_2048.h"

unsigned const char PROGMEM font_2048[18][13] = {
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000} // 0
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00011001, 0b00010101, 0b00010010, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000} // 1
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000111, 0b00000100, 0b00011110, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000} // 2
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001111, 0b00010101, 0b00011110, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000} // 3
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00011111, 0b00000000, 0b00011111, 0b00010101, 0b00011100, 0b00000000, 0b00000000, 0b00000000} // 4
    {0b00000000, 0b00000000, 0b00000000, 0b00010001, 0b00010101, 0b00011010, 0b00000000, 0b00011001, 0b00010101, 0b00010010, 0b00000000, 0b00000000, 0b00000000} // 5
    {0b00000000, 0b00000000, 0b00000000, 0b00011111, 0b00010101, 0b00011100, 0b00000000, 0b00000111, 0b00000100, 0b00011110, 0b00000000, 0b00000000, 0b00000000} // 6
    {0b00000000, 0b00000001, 0b00011111, 0b00000000, 0b00011001, 0b00010101, 0b00010010, 0b00000000, 0b00001111, 0b00010101, 0b00011110, 0b00000000, 0b00000000} // 7
    {0b00000000, 0b00011001, 0b00010101, 0b00010010, 0b00000000, 0b00010111, 0b00010101, 0b00011000, 0b00000000, 0b00011111, 0b00010101, 0b00011100, 0b00000000} // 8
    {0b00000000, 0b00000000, 0b00010111, 0b00010101, 0b00011000, 0b00000001, 0b00011111, 0b00000000, 0b00011001, 0b00010101, 0b00010010, 0b00000000, 0b00000000} // 9
    {0b00000000, 0b00000001, 0b00011111, 0b00001110, 0b00010001, 0b00001110, 0b00011001, 0b00010101, 0b00010110, 0b00000111, 0b00000100, 0b00011110, 0b00000000} // 10
    {0b00000000, 0b00011001, 0b00010101, 0b00010010, 0b00001110, 0b00010001, 0b00001110, 0b00000111, 0b00000100, 0b00011110, 0b00001111, 0b00010101, 0b00011110} // 11
    {0b00000000, 0b00000111, 0b00000100, 0b00011110, 0b00001110, 0b00010001, 0b00001110, 0b00010111, 0b00010101, 0b00011110, 0b00011111, 0b00010101, 0b00011100} // 12
    {0b00000000, 0b00001111, 0b00010101, 0b00011110, 0b00000001, 0b00011111, 0b00000000, 0b00010111, 0b00010101, 0b00011110, 0b00011001, 0b00010101, 0b00010010} // 13
    {0b00000001, 0b00011111, 0b00011111, 0b00010101, 0b00011100, 0b00010001, 0b00010101, 0b00011010, 0b00001111, 0b00010101, 0b00011111, 0b00000100, 0b00011110} // 14
    {0b00011111, 0b00000010, 0b00000100, 0b00011110, 0b00000000, 0b00011100, 0b00010100, 0b00011100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000} // 15 : No
    {0b00000011, 0b00000100, 0b00011100, 0b00000110, 0b00000000, 0b00001110, 0b00010010, 0b00010100, 0b00000000, 0b00010000, 0b00010110, 0b00011010, 0b00000000} // 16 : Yes
    {0b00011111, 0b00010001, 0b00011001, 0b00011111, 0b00000000, 0b00011100, 0b00010000, 0b00011100, 0b00000000, 0b00011010, 0b00000000, 0b00011111, 0b00010100} // 17 : Quit
};
