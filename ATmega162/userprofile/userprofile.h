
#pragma once


#define MAX_NUM_USERS       6
#define MAX_USERNAME_LENGTH 8

// User 0 is reserved for a "default" user, which is reset each time

typedef struct UserProfile UserProfile;
struct UserProfile {
    char        username[MAX_USERNAME_LENGTH];
    struct {
        uint32_t    bestScore;
        uint8_t     grid[4][4];
    } game_2048;
    struct {
        uint8_t settings;
    } game_pong;
};



uint8_t getCurrentUser(void);
void setCurrentUser(uint8_t user);
UserProfile getUserProfile(uint8_t user);
UserProfile getCurrentUserProfile(void);
void writeUserProfile(UserProfile* p, uint8_t user);
void writeCurrentUserProfile(UserProfile* p);
void deleteUserProfile(uint8_t user);
void user_delete(void);
void user_new(void);
void user_login(void);