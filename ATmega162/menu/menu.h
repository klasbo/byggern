
#pragma once

#define MAX_submenus 5

typedef struct menuitem_t menuitem_t;
typedef struct menunode_t menunode_t;

struct menuitem_t {
    const char*     name;
    void            (*fun)(void);
};

struct menunode_t {
    menuitem_t      item;
    int             num_submenus;
    menunode_t*     submenus[MAX_submenus];
    menunode_t*     parent;
};


/** Returns the root node of the menu tree
*/
menunode_t* get_menu(void);


/// ----- OPEN/CLOSE/NEXT/PREV ----- ///

/** Tries to open the submenu at submenu_idx, and returns either:
    The submenu (if it exists), or
    Itself (if it doesn't)
*/
menunode_t* menu_open_submenu(menunode_t* const menu, int submenu_idx);

/** Tries to open the first submenu of menu, and returns either:
    The first submenu (if it exists), or
    Itself (if it doesn't)
    
*/
menunode_t* menu_open(menunode_t* const menu);

/** Closes this menu and returns either:
    Its parent (if it has one), or
    Itself (if it has no parent)
*/
menunode_t* menu_close(menunode_t* const menu);

/** Tries to get the next/prev menu, and returns either:
    The next/prev menu (if it exists), or
    Itself (if it doesn't)
    (Implementation detail: Looks up this menu's parent's submenus)
*/
menunode_t* menu_next(menunode_t* const menu);

/** ditto
*/
menunode_t* menu_prev(menunode_t* const menu);


/// ----- ITERATORS/FOREACH ----- ///

/** Applies func to each parent of menu
    depth is incremented by 1 for each outward step towards the root node
*/
void foreach_parent(menunode_t* const menu, void func(menunode_t* m, int depth));

/** Applies func to each parent, but starting at the root node.
    depth is incremented by 1 for each inward step towards menu
    (Implementation detail: Recursive function)
*/
void foreach_parent_reverse(menunode_t* const menu, void func(menunode_t* m, int depth));

/** Applies func to each next menu
    idx is incremented by 1 for each next menu
    note: idx corresponds to the index in the parent's submenu list (it does not start at 0)
*/
void foreach_nextmenu(menunode_t* const menu, void func(menunode_t* m, int idx));

/** Applies func to each submenu
    idx is incremented by 1 for each next menu
*/
void foreach_submenu(menunode_t* const menu, void func(menunode_t* m, int idx));