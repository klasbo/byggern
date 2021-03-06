
#include <stdint.h>

typedef enum MotorDirection MotorDirection;
enum MotorDirection {
    right,
    left
};

void motor_init(void);
void motor_twiAddr(uint8_t addr);
void motor_speed(uint8_t speed);
void motor_direction(MotorDirection dir);
void motor_velocity(float vel);
int16_t motor_encoderRead(void);
void motor_encoderReset(void);
