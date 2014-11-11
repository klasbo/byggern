
#include <avr/eeprom.h>
#include <stdint.h>
#include <string.h>

#include "userprofile.h"

static uint8_t              currentUser;
static UserProfile EEMEM    userProfiles[MAX_NUM_USERS];

void __attribute__((constructor)) userProfileInit(void){
    UserProfile p;
    memset(&p, 0, sizeof(UserProfile));
    eeprom_write_block(&p, &userProfiles[0], sizeof(UserProfile));
}

uint8_t getCurrentUser(void){
    return currentUser;
}

void setCurrentUser(uint8_t user){
    currentUser = user < MAX_NUM_USERS ? user : currentUser;
}

UserProfile getUserProfile(uint8_t user){
    return userProfiles[user];
}

UserProfile getCurrentUserProfile(void){
    return userProfiles[currentUser];
}

void writeUserProfile(UserProfile* p, uint8_t user){
    if(user > 0  &&  user < MAX_NUM_USERS){
        eeprom_write_block(p, &userProfiles[user], sizeof(UserProfile));
    }
}

void writeCurrentUserProfile(UserProfile* p){
    writeUserProfile(p, currentUser);
}

void deleteUserProfile(uint8_t user){
    if(user > 0  &&  user < MAX_NUM_USERS){
        // memset not available for eeprom?
        UserProfile p;
        memset(&p, 0, sizeof(UserProfile));
        eeprom_write_block(&p, &userProfiles[user], sizeof(UserProfile));
    }
}


