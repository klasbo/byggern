
#pragma once

#define MAX_submenus 5

typedef struct menuitem_t menuitem_t;
typedef struct menunode_t menunode_t;

struct menuitem_t {
    const char*     name;
    void            (*fun)(void*);
};

struct menunode_t {
    menuitem_t      item;
    int             num_submenus;
    menunode_t*     submenus[MAX_submenus];
    menunode_t*     parent;
};


#define for_nextmenu(menu) \
    menunode_t* _prev = NULL; \
    for(; _prev != menu; (_prev = menu), (menu = menu_next(menu)))

#define foreach_submenu(submenu, menu) \
    menunode_t* submenu = menu->submenus[0]; \
    for(int _##menu##_iter = 0; _##menu##_iter < menu->num_submenus; _##menu##_iter++, (submenu = menu->submenus[_##menu##_iter]))


menunode_t* get_menu(void);

/** Tries to open the submenu at submenu_idx, and returns either:
    The submenu (if it exists), or
    Itself (if it doesn't)
*/
menunode_t* menu_open_submenu(menunode_t* menu, int submenu_idx);

/** Tries to open the first submenu of menu, and returns either:
    The first submenu (if it exists), or
    Itself (if it doesn't)
    
*/
menunode_t* menu_open(menunode_t* menu);

/** Closes this menu and returns either:
    Its parent (if it has one), or
    Itself (if it has no parent)
*/
menunode_t* menu_close(menunode_t* menu);

/** Tries to get the next/prev menu, and returns either:
    The next/prev menu (if it exists), or
    Itself (if it doesn't)
    (Implementation detail: looks up this menu's parent's submenus)
*/
menunode_t* menu_next(menunode_t* menu);

/** ditto
*/
menunode_t* menu_prev(menunode_t* menu);