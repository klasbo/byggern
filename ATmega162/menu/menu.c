

#include "menu.h"


static menunode_t* mainMenu = &(menunode_t){
    .item = { .name = "1: main menu",                               },
    .num_submenus = 4,
    .submenus = {
        &(menunode_t){
            .item = { .name = "1-1: settings",                              },
        },
        &(menunode_t){
            .item = { .name = "1-2: things",                                },
        },
        &(menunode_t){
            .item = { .name = "1-3:",                                           },
            .num_submenus = 1,
            .submenus = {
                &(menunode_t){
                    .item = { .name = "1-3-1:",                                     },
                    .num_submenus = 1,
                    .submenus = {
                        &(menunode_t){
                            .item = { .name = "1-3-1-1:",                               },
                        }
                    },
                }
            },
        },
        &(menunode_t){
            .item = { .name = "1-4: asdf",                                  },
            .num_submenus = 1,
            .submenus = {
                &(menunode_t){
                    .item = { .name = "1-4-1:     ",                                },
                }
            },
        },
    },
};

static void menu_init_recursive(menunode_t* m){
    for(int i = 0; i < m->num_submenus; i++){
        m->submenus[i]->parent = m;
        menu_init_recursive(m->submenus[i]);
    }
}

static void __attribute__ ((constructor)) menu_init(void){
    menu_init_recursive(mainMenu);
}

menunode_t* get_menu(void){
    return mainMenu;
}

menunode_t* menu_open_submenu(menunode_t* menu, int submenu_idx){
    if(submenu_idx < menu->num_submenus && menu->submenus[submenu_idx]){
        return menu->submenus[submenu_idx];
    } else {
        return menu;
    }
}

menunode_t* menu_open(menunode_t* menu){
    return menu_open_submenu(menu, 0);
}

menunode_t* menu_close(menunode_t* menu){
    if(menu->parent){
        return menu->parent;
    } else {
        return menu;
    }
}


menunode_t* menu_next(menunode_t* menu){
    if(menu->parent){
        for(int i = 0; i < menu->parent->num_submenus; i++){
            if(menu->parent->submenus[i] == menu &&
               menu->parent->submenus[i+1]){
                return menu->parent->submenus[i+1];
            }
        }
    }
    return menu;
}

menunode_t* menu_prev(menunode_t* menu){
    if(menu->parent){
        for(int i = 1; i < menu->parent->num_submenus; i++){
            if(menu->parent->submenus[i] == menu &&
               menu->parent->submenus[i-1]){
                return menu->parent->submenus[i-1];
            }
        }
    }
    return menu;
}