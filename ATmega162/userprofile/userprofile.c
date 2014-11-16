
#include <avr/eeprom.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>


#include "userprofile.h"
#include "../drivers/analog/joystick.h"
#include "../drivers/analog/slider.h"
#include "../drivers/display/fonts/font8x8.h"
#include "../drivers/display/frame_buffer.h"
#include "../macros.h"


static uint8_t      currentUser;
UserProfile EEMEM   userProfiles[MAX_NUM_USERS];

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
    if(user > 0  &&  user < MAX_NUM_USERS){
        eeprom_write_block(p, &userProfiles[user], sizeof(UserProfile));
    }
}

void writeCurrentUserProfile(UserProfile* p){
    writeUserProfile(p, currentUser);
}

void deleteUserProfile(uint8_t user){
    if(user > 0  &&  user < MAX_NUM_USERS  &&  user != currentUser){
        // memset not available for eeprom
        UserProfile p;
        memset(&p, 0, sizeof(UserProfile));
        eeprom_write_block(&p, &userProfiles[user], sizeof(UserProfile));
    }
}


void renderUsernames(char* info){
    frame_buffer_clear();
    frame_buffer_set_font(font8x8, FONT8x8_WIDTH, FONT8x8_HEIGHT, FONT8x8_START_OFFSET);
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


void settingsIterator(
    char*   actionInfo,
    void    (*renderBackground)(char*),
    uint8_t numItems,
    void    (*action)(uint8_t)
){
    renderBackground(actionInfo);

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
                selected = selected > 0 ?           selected - 1 : selected;
                break;
                case DOWN:
                selected = selected < numItems-1 ?  selected + 1 : selected;
                break;
                default: break;
            }
        }
        // Since opening the item is done with the right button, we need to make 
        //  sure the user has let go before the button can be considered "pressed"
        if(!SLI_get_right_button()){
            selectable = 1;
        }
        if(SLI_get_right_button() && selectable){
            action(selected);
            renderBackground(actionInfo);
        }
        if(SLI_get_left_button()){
            return;
        }
    }
}


void user_login(void){
    settingsIterator("Login As", renderUsernames, MAX_NUM_USERS, setCurrentUser);
}

void user_add(void){
    settingsIterator(
        "Add user",
        renderUsernames,
        MAX_NUM_USERS,
        lambda(void, (uint8_t selected){
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
                    frame_buffer_printf("%-8s", newUser.username);
                    frame_buffer_render();

                    joyDirnPrev = joyDirn;
                    joyDirn = JOY_get_direction();
                    if (joyDirn != joyDirnPrev && joyDirn != NEUTRAL){
                        switch(joyDirn){
                            case DOWN:
                                if(c < 'z'){
                                    c++;
                                }
                                break;
                            case UP:
                                if(c > 'a'){
                                    c--;
                                }
                                break;
                            case LEFT:
                                if(pos > 0){
                                    newUser.username[pos] = 0;
                                    pos--;
                                }
                                c = newUser.username[pos];
                                break;
                            case RIGHT:
                                if(pos < MAX_USERNAME_LENGTH  &&  newUser.username[pos]){
                                    pos++;
                                    c = 'a';
                                }                                                           
                                break;
                            default: break;
                        }
                    }
                    if(!SLI_get_right_button()){
                        quitable = 1;
                    }
                    if(SLI_get_right_button() && quitable){
                        newUser.game_pong.motorInputType    = CONTROL_JOY_X;
                        newUser.game_pong.motorSensitivity  = 3;
                        newUser.game_pong.servoInputType    = CONTROL_SLI_R;
                        newUser.game_pong.solenoidInputType = CONTROL_JOY_UP;
                        writeUserProfile(&newUser, selected);
                        setCurrentUser(selected);
                        return;
                    }
                    if(SLI_get_left_button()){
                        return;
                    }
                }
            }
        })        
    );
}




void user_delete(void){ 
    settingsIterator("Delete", renderUsernames, MAX_NUM_USERS, deleteUserProfile);
}

void user_highscores_pong(void){
    frame_buffer_clear();
    frame_buffer_printf("Highscores");
    for(uint8_t i = 0; i < MAX_NUM_USERS; i++){
        UserProfile p = getUserProfile(i);
        if(p.username[0]){
            frame_buffer_set_cursor(0, (i+1)*FONT8x8_HEIGHT);
            frame_buffer_printf("%-8s: %5d\n", p.username, p.game_pong.bestScore);
        }
    }
    frame_buffer_set_cursor(0, 7*FONT8x8_HEIGHT);
    frame_buffer_printf("[Quit]");
    frame_buffer_render();
    while(1){
        if(SLI_get_left_button()){
            return;
        }
    }
}

void user_highscores_2048(void){
    frame_buffer_clear();
    frame_buffer_printf("Highscores");
    for(uint8_t i = 0; i < MAX_NUM_USERS; i++){
        UserProfile p = getUserProfile(i);
        if(p.username[0]){
            frame_buffer_set_cursor(0, (i+1)*FONT8x8_HEIGHT);
            frame_buffer_printf("%-8s: %5d\n", p.username, p.game_2048.bestScore);
        }
    }
    frame_buffer_set_cursor(0, 7*FONT8x8_HEIGHT);
    frame_buffer_printf("[Quit]");
    frame_buffer_render();
    while(1){
        if(SLI_get_left_button()){
            return;
        }
    }
}



void controls_motor(void){
    settingsIterator(
        "Motor",
        lambda(void, (char* setting){
            frame_buffer_clear();
            frame_buffer_set_font(font8x8, FONT8x8_WIDTH, FONT8x8_HEIGHT, FONT8x8_START_OFFSET);
            frame_buffer_printf("%s\n", setting);
            frame_buffer_printf(
                "  Joy X\n"
                "  Joy Y\n"
                "  Sli R\n"
                "  Sli L"
            );
            
            uint8_t currentControl = getCurrentUserProfile().game_pong.motorInputType;
            if(currentControl != 0){
                frame_buffer_set_cursor(0, currentControl*FONT8x8_HEIGHT);
                frame_buffer_printf("-");
            }
            
            frame_buffer_set_cursor(0, 7*FONT8x8_HEIGHT);
            frame_buffer_printf("[Quit]   [Sel]");
        }),
        4,
        lambda(void, (uint8_t selected){
            UserProfile p = getCurrentUserProfile();
            p.game_pong.motorInputType =
                selected == 0 ? CONTROL_JOY_X :
                selected == 1 ? CONTROL_JOY_Y :
                selected == 2 ? CONTROL_SLI_R :
                selected == 3 ? CONTROL_SLI_L :
                                0 ;
            writeCurrentUserProfile(&p);
        })
    );
}

void controls_motor_sensitivity(void){
    settingsIterator(
        "Sensitivity",
        lambda(void, (char* setting){
            frame_buffer_clear();
            frame_buffer_set_font(font8x8, FONT8x8_WIDTH, FONT8x8_HEIGHT, FONT8x8_START_OFFSET);
            frame_buffer_printf("%s\n", setting);
            frame_buffer_printf(
              "  1\n"
              "  2\n"
              "  3\n"
              "  4\n"
              "  5\n"
            );

            uint8_t currentValue = getCurrentUserProfile().game_pong.motorSensitivity;
            if(currentValue != 0){
                frame_buffer_set_cursor(0, currentValue*FONT8x8_HEIGHT);
                frame_buffer_printf("-");
            }

            frame_buffer_set_cursor(0, 7*FONT8x8_HEIGHT);
            frame_buffer_printf("[Quit]   [Sel]");
        }),
        5,
        lambda(void, (uint8_t val){
            UserProfile p = getCurrentUserProfile();
            p.game_pong.motorSensitivity = val + 1;
            writeCurrentUserProfile(&p);
        })
    );
}

void controls_servo(void){
    settingsIterator(
        "Servo",
        lambda(void, (char* setting){
            frame_buffer_clear();
            frame_buffer_set_font(font8x8, FONT8x8_WIDTH, FONT8x8_HEIGHT, FONT8x8_START_OFFSET);
            frame_buffer_printf("%s\n", setting);
            frame_buffer_printf(
                "  Joy X\n"
                "  Joy Y\n"
                "  Sli R\n"
                "  Sli L"
            );
            
            uint8_t currentControl = getCurrentUserProfile().game_pong.servoInputType;
            if(currentControl != 0){
                frame_buffer_set_cursor(0, currentControl*FONT8x8_HEIGHT);
                frame_buffer_printf("-");
            }
            
            frame_buffer_set_cursor(0, 7*FONT8x8_HEIGHT);
            frame_buffer_printf("[Quit]   [Sel]");
        }),
        4,
        lambda(void, (uint8_t selected){
            UserProfile p = getCurrentUserProfile();
            p.game_pong.servoInputType =
                selected == 0 ? CONTROL_JOY_X :
                selected == 1 ? CONTROL_JOY_Y :
                selected == 2 ? CONTROL_SLI_R :
                selected == 3 ? CONTROL_SLI_L :
                                0 ;
            writeCurrentUserProfile(&p);
        })
    );
}

void controls_solenoid(void){
    settingsIterator(
        "Solenoid",
        lambda(void, (char* setting){
            frame_buffer_clear();
            frame_buffer_set_font(font8x8, FONT8x8_WIDTH, FONT8x8_HEIGHT, FONT8x8_START_OFFSET);
            frame_buffer_printf("%s\n", setting);
            frame_buffer_printf(
                "  Joy Btn\n"
                "  Joy Up\n"
                "  Sli Btn R"
            );
            
            uint8_t currentControl = getCurrentUserProfile().game_pong.solenoidInputType;
            if(currentControl != 0){
                frame_buffer_set_cursor(0, currentControl*FONT8x8_HEIGHT);
                frame_buffer_printf("-");
            }
            
            frame_buffer_set_cursor(0, 7*FONT8x8_HEIGHT);
            frame_buffer_printf("[Quit]   [Sel]");
        }),
        3,
        lambda(void, (uint8_t selected){
            UserProfile p = getCurrentUserProfile();
            p.game_pong.solenoidInputType =
                selected == 0 ? CONTROL_JOY_BTN :
                selected == 1 ? CONTROL_JOY_UP :
                selected == 2 ? CONTROL_SLI_BTN_R :
                                0 ;
            writeCurrentUserProfile(&p);
        })
    );
}
