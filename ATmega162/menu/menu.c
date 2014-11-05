

#include "menu.h"


extern void sram_test(void);
extern void analog_test(void);
extern void can_test(void);
extern void game_2048(void);

static menunode_t* mainMenu = &(menunode_t){
    .item = { .name = "Main Menu",                                  },
    .num_submenus = 3,
    .submenus = {
        &(menunode_t){
            .item = { .name = "Settings",                               },
        },
        &(menunode_t){
            .item = { .name = "Games",                                  },
            .num_submenus = 1,
            .submenus = {
                &(menunode_t){
                    .item = { .name = "2048",       .fun = &game_2048       },
                },
            },
        },
        &(menunode_t){
            .item = { .name = "Tests",                                  },
            .num_submenus = 3,
            .submenus = {
                &(menunode_t){
                    .item = { .name = "SRAM",       .fun = &sram_test       },
                },
                &(menunode_t){
                    .item = { .name = "Analog",     .fun = &analog_test     },
                },
                &(menunode_t){
                    .item = { .name = "CAN",        .fun = &can_test        },
                },
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

void __attribute__ ((constructor)) menu_init(void){
    menu_init_recursive(mainMenu);
}

menunode_t* get_menu(void){
    return mainMenu;
}


/// ----- OPEN/CLOSE/NEXT/PREV ----- ///

menunode_t* menu_open_submenu(menunode_t* const menu, int submenu_idx){
    if(submenu_idx < menu->num_submenus && menu->submenus[submenu_idx]){
        return menu->submenus[submenu_idx];
    } else {
        return menu;
    }
}

menunode_t* menu_open(menunode_t* const menu){
    return menu_open_submenu(menu, 0);
}

menunode_t* menu_close(menunode_t* const menu){
    if(menu->parent /* && menu->parent != mainMenu */){
        return menu->parent;
    } else {
        return menu;
    }
}


menunode_t* menu_next(menunode_t* const menu){
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

menunode_t* menu_prev(menunode_t* const menu){
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


/// ----- ITERATORS/FOREACH ----- ///

void foreach_parent(menunode_t* const menu, void func(menunode_t* m, int depth)){
    menunode_t* curr    = menu;
    menunode_t* prev    = 0;
    int         depth   = 0;

    while(prev != curr){
        func(curr, depth);

        prev = curr;
        curr = menu_close(curr);
        depth++;
    }
}

static int foreach_parent_reverse_impl(menunode_t* const menu, int depth, void func(menunode_t* m, int depth)){
    int a = depth;
    if(menu){
        a = foreach_parent_reverse_impl(menu->parent, depth, func);
        func(menu, a);
        a++;
    }
    return a;
}


void foreach_parent_reverse(menunode_t* const menu, void func(menunode_t* m, int depth)){
    foreach_parent_reverse_impl(menu, 0, func);
}

void foreach_nextmenu(menunode_t* const menu, void func(menunode_t* m, int idx)){
    menunode_t* curr = menu;
    menunode_t* prev = 0;
    int         idx  = 0;
    if(menu->parent){
        for(; idx < menu->parent->num_submenus; idx++){
            if(menu->parent->submenus[idx] == menu){
                break;
            }
        }
    }

    while(prev != curr){
        func(curr, idx);

        prev = curr;
        curr = menu_next(curr);
        idx++;
    }
}

void foreach_submenu(menunode_t* const menu, void func(menunode_t* m, int idx)){
    menunode_t* sub = menu_open(menu);
    if(sub != menu){
        foreach_nextmenu(sub, func);
    }
}