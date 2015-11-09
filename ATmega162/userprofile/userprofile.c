
#include <avr/eeprom.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>


#include "userprofile.h"
#include "../drivers/analog/joystick.h"
#include "../drivers/analog/slider.h"
#include "../drivers/display/fonts/font5x7w.h"
#include "../drivers/display/frame_buffer.h"
#include "../drivers/display/oled.h"
#include "../../lib/macros.h"


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



static void renderUsernamesBackground(const char* title){
    fbuf_printf("%s\n", title);
    for(uint8_t i = 0; i < MAX_NUM_USERS; i++){
        if(i == getCurrentUser()){
            fbuf_printf("-");
        }
        fbuf_set_cursor(2, i+1);
        fbuf_printf("%s\n", getUserProfile(i).username);
    }
    fbuf_printf("[Quit]      [Sel]");
    fbuf_render();
}

static void renderLeftButton(const char* title){
    fbuf_set_cursor_to_pixel(0, DISP_HEIGHT-8);
    fbuf_printf("[%s]", title);
}

static void renderRightButton(const char* title){
    fbuf_set_cursor_to_pixel(
        DISP_WIDTH  -  fbuf_get_current_font_config().width * (strlen(title)+2), 
        DISP_HEIGHT - 8
    );
    fbuf_printf("[%s]", title);
}


static void settingsIterator(
    void    (*renderBackground)(void),
    uint8_t numItems,
    void    (*action)(uint8_t)
){
    fbuf_set_font(font5x7w());
    fbuf_set_font_spacing(1, 1);
    fbuf_clear();
    renderBackground();

    JOY_dir_t joyDirnPrev   = NEUTRAL;
    JOY_dir_t joyDirn       = NEUTRAL;
    
    uint8_t selected = 0;
    uint8_t SLIRightButtonReleased = 0;
    
    while(1){
        fbuf_set_cursor(1, selected+1);
        fbuf_printf(">");
        fbuf_render();

        joyDirnPrev = joyDirn;
        joyDirn = joystick_direction();
        if (joyDirn != joyDirnPrev && joyDirn != NEUTRAL){
            fbuf_set_cursor(1, selected+1);
            fbuf_printf(" ");
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
        if(!slider_right_button()){
            SLIRightButtonReleased = 1;
        }
        if(slider_right_button() && SLIRightButtonReleased){
            action(selected);
            renderBackground();
        }
        if(slider_left_button()){
            return;
        }
    }
}


void user_login(void){
    settingsIterator(
        lambda(void, (void){
            renderUsernamesBackground("Login As");
        }),
        MAX_NUM_USERS,
        setCurrentUser
    );
}

void user_add(void){
    settingsIterator(
        lambda(void, (void){
            renderUsernamesBackground("Add User");
        }),
        MAX_NUM_USERS,
        lambda(void, (uint8_t selected){
            if(getUserProfile(selected).username[0] == 0){ // if this user does not exist
                renderLeftButton("Back");
                renderRightButton("Ok");
                fbuf_render();
                UserProfile newUser = getUserProfile(selected);

                // set default config
                newUser.game_pong.motorInputType    = CONTROL_JOY_X;
                newUser.game_pong.motorSensitivity  = 3;
                newUser.game_pong.servoInputType    = CONTROL_SLI_R;
                newUser.game_pong.servoReversed     = 0;
                newUser.game_pong.solenoidInputType = CONTROL_JOY_UP;
                newUser.game_pong.useBluetooth      = 0;

                // edit newUser.username
                uint8_t     SLIRightButtonReleased  = 0;
                char        c                       = 'a';
                uint8_t     pos                     = 0;
                JOY_dir_t   joyDirnPrev             = NEUTRAL;
                JOY_dir_t   joyDirn                 = NEUTRAL;
                while(1){
                    newUser.username[pos] = c;
                    fbuf_set_cursor(2, selected+1);
                    fbuf_printf("%-8s", newUser.username);
                    fbuf_render();

                    joyDirnPrev = joyDirn;
                    joyDirn = joystick_direction();
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
                    if(!slider_right_button()){
                        SLIRightButtonReleased = 1;
                    }
                    if(slider_right_button() && SLIRightButtonReleased){
                        writeUserProfile(&newUser, selected);
                        setCurrentUser(selected);
                        return;
                    }
                    if(slider_left_button()){
                        return;
                    }
                }
            }
        })        
    );
}

void user_delete(void){ 
    settingsIterator(
        lambda(void, (void){
            renderUsernamesBackground("Delete");
        }),
        MAX_NUM_USERS,
        deleteUserProfile
    );
}



void user_highscores_pong(void){
    fbuf_clear();
    fbuf_printf("Highscores");
    for(uint8_t i = 0; i < MAX_NUM_USERS; i++){
        UserProfile p = getUserProfile(i);
        if(p.username[0]){
            fbuf_set_cursor(0, i+1);
            fbuf_printf("%-8s: %5d\n", p.username, p.game_pong.bestScore);
        }
    }
    renderLeftButton("[Quit]");
    fbuf_render();
    while(1){
        if(slider_left_button()){
            return;
        }
    }
}

void user_highscores_2048(void){
    fbuf_clear();
    fbuf_printf("Highscores");
    for(uint8_t i = 0; i < MAX_NUM_USERS; i++){
        UserProfile p = getUserProfile(i);
        if(p.username[0]){
            fbuf_set_cursor(0, i+1);
            fbuf_printf("%-8s: %5lu\n", p.username, p.game_2048.bestScore);
        }
    }
    renderLeftButton("[Quit]");
    fbuf_render();
    while(1){
        if(slider_left_button()){
            return;
        }
    }
}



void renderControlsBackground(char * text, uint8_t selected){
    fbuf_printf(text);

    if(selected != 0){
        fbuf_set_cursor(0, selected);
        fbuf_printf("-");
    }
            
    renderLeftButton("Quit");
    renderRightButton("Sel");
}


void controls_motor(void){
    settingsIterator(
        lambda(void, (void){
            renderControlsBackground(
                "Motor\n"
                "  Joy X\n"
                "  Joy Y\n"
                "  Sli R\n"
                "  Sli L",
                getCurrentUserProfile().game_pong.motorInputType
            );
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

void controls_motorSensitivity(void){
    settingsIterator(
        lambda(void, (void){
            renderControlsBackground(
                "Sensitivity\n"
                "  1\n"
                "  2\n"
                "  3\n"
                "  4\n"
                "  5\n",
                getCurrentUserProfile().game_pong.motorSensitivity
            );
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
        lambda(void, (void){
            renderControlsBackground(
                "Servo\n"
                "  Joy X\n"
                "  Joy Y\n"
                "  Sli R\n"
                "  Sli L",
                getCurrentUserProfile().game_pong.servoInputType
            );
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

void controls_servoReversed(void){
    settingsIterator(
        lambda(void, (void){
            renderControlsBackground(
                "Servo dir\n"
                "  Normal\n"
                "  Reversed\n",
                getCurrentUserProfile().game_pong.servoReversed
            );
        }),
        2,
        lambda(void, (uint8_t val){
            UserProfile p = getCurrentUserProfile();
            p.game_pong.servoReversed = val;
            writeCurrentUserProfile(&p);
        })
    );
}

void controls_solenoid(void){
    settingsIterator(
        lambda(void, (void){
            renderControlsBackground(
                "Solenoid\n"    
                "  Joy Btn\n"
                "  Joy Up\n"
                "  Sli Btn R",
                getCurrentUserProfile().game_pong.solenoidInputType
            );
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

void controls_bluetooth(void){
    settingsIterator(
        lambda(void, (void){
            renderControlsBackground(
                "Bluetooth\n"            
                "  Off\n"
                "  On\n",
                getCurrentUserProfile().game_pong.useBluetooth
            );
        }),
        2,
        lambda(void, (uint8_t val){
            UserProfile p = getCurrentUserProfile();
            p.game_pong.useBluetooth = val;
            writeCurrentUserProfile(&p);
        })
    );
}
