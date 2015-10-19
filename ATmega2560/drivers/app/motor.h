
#include <stdint.h>

typedef enum MotorDirection MotorDirection;
enum MotorDirection {
    right,
    left
};

void motor_init(void);
void motor_twi_addr(uint8_t addr);
void motor_speed(uint8_t speed);
void motor_direction(MotorDirection dir);
int16_t motor_encoder_read(void);
void motor_encoder_counter_reset(void);