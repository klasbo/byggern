
#include <avr/eeprom.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>


#include "userprofile.h"
#include "../drivers/analog/joystick.h"
#include "../drivers/analog/slider.h"
#include "../drivers/display/fonts/font8x8.h"
#include "../drivers/display/frame_buffer.h"


static uint8_t              currentUser;
UserProfile EEMEM    userProfiles[MAX_NUM_USERS];

void createDefaultProfile(void){
    UserProfile p;
    memset(&p, 0, sizeof(UserProfile));
    sprintf(p.username, "default");
    eeprom_write_block(&p, &userProfiles[0], sizeof(UserProfile));
}

uint8_t getCurrentUser(void){
    return currentUser;
}

void setCurrentUser(uint8_t user){
    currentUser = user < MAX_NUM_USERS ? user : currentUser;
}

UserProfile getUserProfile(uint8_t user){
    UserProfile p;
    eeprom_read_block(&p, &userProfiles[user], sizeof(UserProfile));
    return p;
}

UserProfile getCurrentUserProfile(void){
    return getUserProfile(getCurrentUser());
}

void writeUserProfile(UserProfile* p, uint8_t user){
    printf("writing %s as user %d\n", p->username, user);
    if(user > 0  &&  user < MAX_NUM_USERS){
        eeprom_write_block(p, &userProfiles[user], sizeof(UserProfile));
    }
}

void writeCurrentUserProfile(UserProfile* p){
    writeUserProfile(p, currentUser);
}

void deleteUserProfile(uint8_t user){
    if(user > 0  &&  user < MAX_NUM_USERS  &&  user != currentUser){
        // memset not available for eeprom?
        UserProfile p;
        memset(&p, 0, sizeof(UserProfile));
        eeprom_write_block(&p, &userProfiles[user], sizeof(UserProfile));
    }
}





void userIterator(char* info, void function(uint8_t selected)){
    void renderUsernames(char* info){
        frame_buffer_clear();
        frame_buffer_set_font(font8x8, FONT8x8_WIDTH, FONT8x8_HEIGHT, FONT8x8_START_OFFSET);
        //frame_buffer_set_font_spacing(0, 0);
        frame_buffer_printf("%s\n", info);
        for(uint8_t i = 0; i < MAX_NUM_USERS; i++){
            if(i == getCurrentUser()){
                frame_buffer_printf("-");
            }
            frame_buffer_set_cursor(2*FONT8x8_WIDTH, (i+1) * FONT8x8_HEIGHT);
            frame_buffer_printf("%s\n", getUserProfile(i).username);
        }
        frame_buffer_printf("[Quit]   [Sel]");
        frame_buffer_render();
    }

    renderUsernames(info);
    
    JOY_dir_t joyDirnPrev   = NEUTRAL;
    JOY_dir_t joyDirn       = NEUTRAL;
    
    uint8_t selected = 0;
    uint8_t selectable = 0;
    
    while(1){
        frame_buffer_set_cursor(1*FONT8x8_WIDTH, (selected+1) * FONT8x8_HEIGHT);
        frame_buffer_printf(">");
        frame_buffer_render();

        joyDirnPrev = joyDirn;
        joyDirn = JOY_get_direction();
        if (joyDirn != joyDirnPrev && joyDirn != NEUTRAL){
            frame_buffer_set_cursor(1*FONT8x8_WIDTH, (selected+1) * FONT8x8_HEIGHT);
            frame_buffer_printf(" ");
            switch(joyDirn){
                case UP:
                    selected = selected > 0 ?                selected - 1 : selected;
                    break;
                case DOWN:
                    selected = selected < MAX_NUM_USERS-1 ?  selected + 1 : selected;
                    break;
                default: break;
            }
        }
        if(!SLI_get_right_button()){
            selectable = 1;
        }
        if(SLI_get_right_button() && selectable){
            function(selected);
            renderUsernames(info);
        }
        if(SLI_get_left_button()){
            return;
        }
    }
}


void user_delete(void){ 
    userIterator("Delete", deleteUserProfile);
}


void user_new(void){
    userIterator("New user",
        ({
        void fn(uint8_t selected){
            if(getUserProfile(selected).username[0] == 0){ // if this user does not exist
                frame_buffer_set_cursor(0, 7*FONT8x8_HEIGHT);
                frame_buffer_printf("[Back]    [Ok]");
                frame_buffer_render();
                UserProfile newUser = getUserProfile(selected);
                // edit newUser.username
                uint8_t     quitable        = 0;
                char        c               = 'a';
                uint8_t     pos             = 0;
                JOY_dir_t   joyDirnPrev     = NEUTRAL;
                JOY_dir_t   joyDirn         = NEUTRAL;
                while(1){
                    newUser.username[pos] = c;
                    frame_buffer_set_cursor(2*FONT8x8_WIDTH, (selected+1) * FONT8x8_HEIGHT);
                    frame_buffer_printf("%s", newUser.username);
                    frame_buffer_render();

                    joyDirnPrev = joyDirn;
                    joyDirn = JOY_get_direction();
                    if (joyDirn != joyDirnPrev && joyDirn != NEUTRAL){
                        switch(joyDirn){
                            case DOWN:
                                if(c == 0){
                                    c = 'a';
                                } else if(c < 'z'){
                                    c++;
                                }
                                break;
                            case UP:
                                if(c == 'a'){
                                    c = 0;
                                } else if(c > 'a'){
                                    c--;
                                }
                                break;
                            case LEFT:
                                pos = pos > 0 ? pos - 1 : pos;
                                break;
                            case RIGHT:
                                pos = pos < MAX_USERNAME_LENGTH ? pos + 1 : pos;
                                break;
                            default: break;
                        }
                    }
                    if(!SLI_get_right_button()){
                        quitable = 1;
                    }
                    if(SLI_get_right_button() && quitable){
                        writeUserProfile(&newUser, selected);
                        setCurrentUser(selected);
                        return;
                    }
                    if(SLI_get_left_button()){
                        return;            
                    }
                }
            }
        }
        &fn;
        })
    );
}


void user_login(void){
    userIterator("Login as", setCurrentUser);
}

