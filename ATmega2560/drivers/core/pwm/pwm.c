#include "pwm.h"

void pwm_period(float period_ms){
    
    // Set Fast PWM (mode 14 from table 17-2)
    TCCR4A  |=  (1 << WGM41);
    TCCR4B  |=  (1 << WGM42)
            |   (1 << WGM43);

            
    // With Fast PWM: clear on cmp match, set at BOTTOM (table 17-4)
    TCCR4A  |=  (1 << COM4A1);


    // 1/64 prescaler
    TCCR4B  |=  (1 << CS41)
            |   (1 << CS40);

    // Set TOP value
    ICR4 = F_CPU/64/1000 * period_ms;

    // Set PH3 / OC4A / Arduino pin 6 as output
    DDRH    |=  (1 << DDH3);

}

void pwm_width(float poswidth_ms){
    OCR4A = F_CPU/64/1000 * poswidth_ms;
}