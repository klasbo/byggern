

#include "menu.h"


extern void sram_test(void);
extern void analog_test(void);
extern void can_test_loopback(void);
extern void game_2048(void);
extern void game_pong(void);
extern void user_login(void);
extern void user_add(void);
extern void user_delete(void);
extern void user_highscores_pong(void);
extern void user_highscores_2048(void);
extern void controls_motor(void);
extern void controls_motorSensitivity(void);
extern void controls_servo(void);
extern void controls_servoReversed(void);
extern void controls_solenoid(void);
extern void controls_bluetooth(void);

static menunode_t* mainMenu;

static uint8_t menu_initialized;

menunode_t* new_menu(char* name, void (*fun)(void), int8_t num_submenus){
    menunode_t* m   = malloc(sizeof(menunode_t));
    if(num_submenus){
        m->submenus     = malloc(sizeof(menunode_t*) * num_submenus);
    }
    m->num_submenus = num_submenus;
    m->item.name    = name;
    m->item.fun     = fun;

    return m;
}

static void menu_assign_parents_recursive(menunode_t* m){
    for(int8_t i = 0; i < m->num_submenus; i++){
        m->submenus[i]->parent = m;
        menu_assign_parents_recursive(m->submenus[i]);
    }
}

void menu_create(void){
    mainMenu = new_menu("Main Menu", NULL, 3);
    mainMenu->submenus[0] = new_menu("Users", NULL, 4);
    mainMenu->submenus[0]->submenus[0] = new_menu("Login",          &user_login,    0);
    mainMenu->submenus[0]->submenus[1] = new_menu("Add User",       &user_add,      0);
    mainMenu->submenus[0]->submenus[2] = new_menu("Delete User",    &user_delete,   0);
    mainMenu->submenus[0]->submenus[3] = new_menu("Highscores",     NULL, 2);
    mainMenu->submenus[0]->submenus[3]->submenus[0] = new_menu("Pong", &user_highscores_pong, 0);
    mainMenu->submenus[0]->submenus[3]->submenus[1] = new_menu("2048", &user_highscores_2048, 0);
    mainMenu->submenus[1] = new_menu("Controls", NULL, 6);
    mainMenu->submenus[1]->submenus[0] = new_menu("Motor",          &controls_motor,                0);
    mainMenu->submenus[1]->submenus[1] = new_menu("Mtr Sens",       &controls_motorSensitivity,     0);
    mainMenu->submenus[1]->submenus[2] = new_menu("Servo",          &controls_servo,                0);
    mainMenu->submenus[1]->submenus[3] = new_menu("Reverse Servo",  &controls_servoReversed,        0);
    mainMenu->submenus[1]->submenus[4] = new_menu("Solenoid",       &controls_solenoid,             0);
    mainMenu->submenus[1]->submenus[5] = new_menu("Bluetooth",      &controls_bluetooth,            0);
    mainMenu->submenus[2] = new_menu("Games", NULL, 2);
    mainMenu->submenus[2]->submenus[0] = new_menu("Pong",           &game_pong,     0);
    mainMenu->submenus[2]->submenus[1] = new_menu("2048",           &game_2048,     0);
    //mainMenu->submenus[2] = new_menu("Tests", NULL, 3);
    //mainMenu->submenus[2]->submenus[0] = new_menu("SRAM",           &sram_test,     0);
    //mainMenu->submenus[2]->submenus[1] = new_menu("Analog",         &analog_test,   0);
    //mainMenu->submenus[2]->submenus[2] = new_menu("CAN",            &can_test_loopback,      0);
    
    menu_assign_parents_recursive(mainMenu);
}

menunode_t* get_menu(void){
    if(!menu_initialized){
        menu_create();
        menu_initialized = 1;
    }
    return mainMenu;
}

int8_t menu_depth(menunode_t* const menu){
    menunode_t* m       = menu;
    int8_t      depth   = 0;

    while(m->parent != NULL){
        m = m->parent;
        depth++;
    }
    return depth;
}

int8_t menu_index(menunode_t* const menu){
    if(menu_close(menu) == menu){
        return 0;
    }
    menunode_t* m       = menu;
    menunode_t* first   = menu_open(menu_close(menu));
    int8_t      idx     = 0;
    while(m != first){
        m = menu_prev(m);
        idx++;
    }
    return idx;
}

/// ----- OPEN/CLOSE/NEXT/PREV ----- ///

menunode_t* menu_open_submenu(menunode_t* const menu, int8_t submenu_idx){
    if(submenu_idx < menu->num_submenus){
        return menu->submenus[submenu_idx];
    } else {
        return menu;
    }
}

menunode_t* menu_open(menunode_t* const menu){
    return menu_open_submenu(menu, 0);
}

menunode_t* menu_close(menunode_t* const menu){
    if(menu->parent){
        return menu->parent;
    } else {
        return menu;
    }
}


menunode_t* menu_next(menunode_t* const menu){
    if(menu->parent){
        for(int8_t i = 0; i < menu->parent->num_submenus - 1; i++){
            if(menu->parent->submenus[i] == menu){
                return menu->parent->submenus[i+1];
            }
        }
    }
    return menu;
}

menunode_t* menu_prev(menunode_t* const menu){
    if(menu->parent){
        for(int8_t i = 1; i < menu->parent->num_submenus; i++){
            if(menu->parent->submenus[i] == menu){
                return menu->parent->submenus[i-1];
            }
        }
    }
    return menu;
}


/// ----- ITERATORS/FOREACH ----- ///

void foreach_parent(menunode_t* const menu, void func(menunode_t* m, int8_t depth)){
    menunode_t* curr    = menu;
    menunode_t* prev    = 0;
    int8_t      depth   = 0;

    while(prev != curr){
        func(curr, depth);

        prev = curr;
        curr = menu_close(curr);
        depth++;
    }
}

static int8_t foreach_parent_reverse_impl(menunode_t* const menu, int8_t depth, void func(menunode_t* m, int8_t depth)){
    int8_t a = depth;
    if(menu){
        a = foreach_parent_reverse_impl(menu->parent, depth, func);
        func(menu, a);
        a++;
    }
    return a;
}


void foreach_parent_reverse(menunode_t* const menu, void func(menunode_t* m, int8_t depth)){
    foreach_parent_reverse_impl(menu, 0, func);
}

void foreach_nextmenu(menunode_t* const menu, void func(menunode_t* m, int8_t idx)){
    menunode_t* curr = menu;
    menunode_t* prev = 0;
    int8_t      idx  = 0;
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

void foreach_submenu(menunode_t* const menu, void func(menunode_t* m, int8_t idx)){
    menunode_t* sub = menu_open(menu);
    if(sub != menu){
        foreach_nextmenu(sub, func);
    }
}