
#pragma once


// User 0 is reserved for a "default" user, which cannot be saved to

#define MAX_NUM_USERS       6
#define MAX_USERNAME_LENGTH 8

#define CONTROL_JOY_X 1
#define CONTROL_JOY_Y 2
#define CONTROL_SLI_R 3
#define CONTROL_SLI_L 4

#define CONTROL_JOY_BTN    1
#define CONTROL_JOY_UP     2
#define CONTROL_SLI_BTN_R  3


typedef struct UserProfile UserProfile;
struct UserProfile {
    char        username[MAX_USERNAME_LENGTH];
    struct {
        uint32_t    bestScore;
        uint8_t     grid[4][4];
    } game_2048;
    struct {
        uint8_t     motorInputType;
        uint8_t     motorSensitivity;
        uint8_t     servoInputType;
        uint8_t     solenoidInputType;
        uint16_t    bestScore;
    } game_pong;
};


// UserProfile functions
uint8_t getCurrentUser(void);
void setCurrentUser(uint8_t user);
UserProfile getUserProfile(uint8_t user);
UserProfile getCurrentUserProfile(void);
void writeUserProfile(UserProfile* p, uint8_t user);
void writeCurrentUserProfile(UserProfile* p);
void deleteUserProfile(uint8_t user);

// Menu functions
void user_delete(void);
void user_add(void);
void user_login(void);
void controls_motor(void);
void controls_motor_sensitivity(void);
void controls_servo(void);
void controls_solenoid(void);