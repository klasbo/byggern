#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <avr/io.h>

#include "../drivers/analog/joystick.h"
#include "../drivers/analog/slider.h"
#include "../drivers/display/frame_buffer.h"
#include "../drivers/display/fonts/font8x8.h"
#include "../drivers/display/fonts/font_2048.h"
#include "../userprofile/userprofile.h"


#define SIZE            4
#define NUM_START_TILES 2
#define GRID_SIZE_X     16
#define GRID_SIZE_Y     15

uint8_t traversal_asc[SIZE] = {0, 1, 2, 3};
uint8_t traversal_dsc[SIZE] = {3, 2, 1, 0};

#define TILE_2048       16  // The tile value needed to win (2048 by default)

typedef struct Position         Position;
typedef struct FarthestPosition FarthestPosition;
typedef struct Tile             Tile;
typedef struct Grid             Grid;
typedef struct Actuator         Actuator;
typedef struct ActuatorMetadata ActuatorMetadata;
#define GameManager_scoped __attribute__((cleanup(delete_GameManager))) GameManager
typedef struct GameManager      GameManager;

/** A non-specific x-y coordinate
*/
struct Position {
    int8_t      x;
    int8_t      y;
};

/** Used when moving a tile
*   A tile is either merged into next, or moved to farthest
*/
struct FarthestPosition {
    Position    farthest;
    Position    next;
};


/** A tile is placed on the grid, and always has a value
*   The actual value is 2^value, to conserve space and to align with font rendering
*/
struct Tile {
    uint8_t     value;
    Position    position;
    Position    previousPosition;
    uint8_t     mergedFrom;
};


/** The grid consists of a SIZE-by-SIZE array of cells
*   Each cell may contain a single Tile (may be NULL)
*/
struct Grid {
    Tile* tiles[SIZE][SIZE];
};

/** The Actuator displays the game state on your thing of choice (OLED display, printf to UART, etc)
*/
struct Actuator {
    void        (*actuate)(Grid const * const g, ActuatorMetadata const am);
};
#define ActuatorTypeFrameBuffer 1
#define ActuatorTypeUART        2

/** A collection of extra stuff the Actuator needs
*/
struct ActuatorMetadata {
    uint32_t    score;
    uint32_t    bestScore;
    uint8_t     over;
    uint8_t     won;
    uint8_t     terminated;
};

/** The GameManager is the core of the game
*/
struct GameManager {
    UserProfile*        userProfile;
    Actuator*           actuator;
    Grid*               grid;
    
    uint8_t             numStartTiles;
    uint32_t            score;
    uint8_t             over;
    uint8_t             won;
    uint8_t             keepPlaying;
};

/** Directions to move the grid in
*/
typedef enum {
    dir_up,
    dir_right,
    dir_down,
    dir_left
} Direction;

/** Intended as a unit vector for iterating in a specific Direction
*/
typedef struct Vector Vector;
struct Vector {
    int8_t      x;
    int8_t      y;
};

// --- POSITION FUNCTIONS --- //
uint8_t withinBounds(Position const p);
uint8_t positionsEqual(Position const first, Position const second);
// --- TILE FUNCTIONS --- //
Tile* new_Tile(Position const p, uint8_t value);
void savePosition(Tile* const t);
void updatePosition(Tile* const t, Position const p);
// --- GRID FUNCTIONS --- //
Grid* new_Grid();
uint8_t cellsAvailable(Grid const * const g);
Tile* cellContent(Grid const * const g, Position const p);
uint8_t cellOccupied(Grid const * const g, Position const p);
void insertTile(Grid* const g, Tile* const t);
void removeTile(Grid* const g, Tile const * const t);
// --- USERPROFILE FUNCTIONS --- //
uint32_t getBestScore(UserProfile const * const p);
void setBestScore(UserProfile* const p, uint32_t const score);
void setGameState(UserProfile* const p, Grid const * const g);
void clearGameState(UserProfile* const p);
// --- GAME MANAGER FUNCTIONS --- //
GameManager* new_GameManager();
void delete_GameManager(GameManager* const * const gm);
uint8_t isGameTerminated(GameManager const * const gm);
void addStartTiles(GameManager* const gm);
void addRandomTile(GameManager* const gm);
Position randomAvailablePosition(GameManager const * const gm);
void actuate(GameManager* const gm);
void prepareTiles(GameManager* const gm);
void moveTile(GameManager* const gm, Tile* const t, Position const p);
void move(GameManager* const gm, Direction const d);
FarthestPosition findFarthestPosition(GameManager const * const gm, Position p, Vector const v);
uint8_t movesAvailable(GameManager const * const gm);
uint8_t tileMatchesAvailable(GameManager const * const gm);
// --- VECTOR FUNCTIONS --- //
Vector getVector(Direction const d);
// --- ACTUATOR FUNCTIONS --- //
Actuator* new_Actuator(void);
void actuateFrameBuffer(Grid const * const g, ActuatorMetadata const am);
void actuateUART(Grid const * const g, ActuatorMetadata const am);
// --- MISC FUNCTIONS --- //
void drawBackground();



// --- POSITION FUNCTIONS --- //
uint8_t withinBounds(Position const p){
    return 
        p.x     >=  0       &&
        p.y     >=  0       &&
        p.x     <   SIZE    &&
        p.y     <   SIZE    ;
}

uint8_t positionsEqual(Position const first, Position const second){
    return first.x == second.x  &&  first.y == second.y;
}


// --- TILE FUNCTIONS --- //
Tile* new_Tile(Position const p, uint8_t value){
    Tile* t = malloc(sizeof(Tile));
    memset(t, 0, sizeof(Tile));
    t->position = p;
    t->value    = value;
    return t;
}

void savePosition(Tile* const t){
    t->previousPosition = t->position;
}

void updatePosition(Tile* const t, Position const p){
    t->position = p;
}


// --- GRID FUNCTIONS --- //
Grid* new_Grid(){
    Grid* g = malloc(sizeof(Grid));
    memset(g, 0, sizeof(Grid));
    return g;    
}

uint8_t cellsAvailable(Grid const * const g){
    for(int x = 0; x < SIZE; x++){
        for(int y = 0; y < SIZE; y++){
            if(!g->tiles[x][y]){
                return 1;
            }
        }
    }
    return 0;
}

Tile* cellContent(Grid const * const g, Position const p){
    if(withinBounds(p)){
        return g->tiles[p.x][p.y];
    } else {
        return NULL;
    }
}

uint8_t cellOccupied(Grid const * const g, Position const p){
    return !!g->tiles[p.x][p.y];
}

void insertTile(Grid* const g, Tile* const t){
    g->tiles[t->position.x][t->position.y] = t;
}

void removeTile(Grid* const g, Tile const * const t){
    g->tiles[t->position.x][t->position.y] = NULL;
}


// --- USERPROFILE FUNCTIONS --- //
uint32_t getBestScore(UserProfile const * const p){
    return p->game_2048.bestScore;
}

void setBestScore(UserProfile* const p, uint32_t const score){
    p->game_2048.bestScore = score;
}

void setGameState(UserProfile* const p, Grid const * const g){
    for(int x = 0; x < SIZE; x++){
        for(int y = 0; y < SIZE; y++){
            if(g->tiles[x][y]){
                p->game_2048.grid[x][y] = g->tiles[x][y]->value;
            } else {
                p->game_2048.grid[x][y] = 0;
            }
        }
    }
}

void clearGameState(UserProfile* const p){
    for(int x = 0; x < SIZE; x++){
        for(int y = 0; y < SIZE; y++){
            p->game_2048.grid[x][y] = 0;
        }
    }
}


// --- GAME MANAGER FUNCTIONS --- //
GameManager* new_GameManager(){
    GameManager* gm = malloc(sizeof(GameManager));
    memset(gm, 0, sizeof(GameManager));
    
    gm->grid            = new_Grid();
    gm->numStartTiles   = NUM_START_TILES;
    gm->actuator        = new_Actuator();
    gm->userProfile     = malloc(sizeof(UserProfile));
    *gm->userProfile    = getCurrentUserProfile();
    
    uint8_t foundStoredGame = 0;

    for(int x = 0; x < SIZE; x++){
        for(int y = 0; y < SIZE; y++){
            if(gm->userProfile->game_2048.grid[x][y]){
                Tile* t = new_Tile(
                    (Position){.x = x, .y = y}, 
                    gm->userProfile->game_2048.grid[x][y]
                );
                insertTile(gm->grid, t);
                foundStoredGame = 1;
            }
        }
    }

    if(!foundStoredGame){
        addStartTiles(gm);
    }
    
    return gm;
}

void delete_GameManager(GameManager* const * const gm){
    for(int x = 0; x < SIZE; x++){
        for(int y = 0; y < SIZE; y++){
            if((*gm)->grid->tiles[x][y]){
                free((*gm)->grid->tiles[x][y]);
            }
        }
    }
    free((*gm)->grid);
    free((*gm)->actuator);
    free((*gm)->userProfile);
    free(*gm);
    return;
}

uint8_t isGameTerminated(GameManager const * const gm){
    return gm->over || (gm->won && !gm->keepPlaying);
}

void addStartTiles(GameManager* const gm){
    for(uint8_t i = 0; i < gm->numStartTiles; i++){
        addRandomTile(gm);
    }
}

void addRandomTile(GameManager* const gm){
    Tile* t = new_Tile(randomAvailablePosition(gm), ((rand() % 10) < 9) ? 1 : 2);
    insertTile(gm->grid, t);
}

Position randomAvailablePosition(GameManager const * const gm){
    Position p;
    do {
        p.x = rand() % SIZE;
        p.y = rand() % SIZE;
    } while(cellOccupied(gm->grid, p));
    return p;
}

void actuate(GameManager* const gm){
    
    if(getBestScore(gm->userProfile) < gm->score){
        setBestScore(gm->userProfile, gm->score);
    }
    
    
    if(isGameTerminated(gm)){
        clearGameState(gm->userProfile);
    } else {
        setGameState(gm->userProfile, gm->grid);
    }
    

    gm->actuator->actuate(
        gm->grid,
        (ActuatorMetadata){
            .score      = gm->score,
            .over       = gm->over,
            .won        = gm->won,
            .bestScore  = getBestScore(gm->userProfile),
            .terminated = isGameTerminated(gm),
        }
    );
}

void prepareTiles(GameManager* const gm){
    for(int x = 0; x < SIZE; x++){
        for(int y = 0; y < SIZE; y++){
            if(gm->grid->tiles[x][y]){
                gm->grid->tiles[x][y]->mergedFrom = 0;
                savePosition(gm->grid->tiles[x][y]);
            }
        }
    }
}

void moveTile(GameManager* const gm, Tile* const t, Position const p){
    gm->grid->tiles[t->position.x][t->position.y] = NULL;
    gm->grid->tiles[p.x][p.y] = t;
    updatePosition(t, p);
}

void move(GameManager* const gm, Direction const d){
    if(isGameTerminated(gm)) return;
    
    Vector      v   = getVector(d);
    Position    p;
    Tile*       t;
    
    uint8_t traversals_x[4];
    uint8_t traversals_y[4];
    memcpy(&traversals_x, &traversal_asc, SIZE);
    memcpy(&traversals_y, &traversal_asc, SIZE);
    if(v.x == 1){
        memcpy(&traversals_x, &traversal_dsc, SIZE);
    }
    if(v.y == 1){
        memcpy(&traversals_y, &traversal_dsc, SIZE);
    }
    
    uint8_t moved = 0;
    
    prepareTiles(gm);
    
    //printf("traversals_x = [%d, %d, %d, %d]\n", traversals_x[0], traversals_x[1], traversals_x[2], traversals_x[3]);
    //printf("traversals_y = [%d, %d, %d, %d]\n", traversals_y[0], traversals_y[1], traversals_y[2], traversals_y[3]);
    
    for(int ix = 0; ix < SIZE; ix++){
        for(int iy = 0; iy < SIZE; iy++){
            p = (Position){.x = traversals_x[ix], .y = traversals_y[iy]};
            t = cellContent(gm->grid, p);
            
            if(t){
                FarthestPosition    fp      = findFarthestPosition(gm, p, v);
                Tile*               next    = cellContent(gm->grid, fp.next);
                
                if( next  &&  
                    next->value == t->value  &&  
                    !next->mergedFrom
                ){
                    Tile* merged = new_Tile(fp.next, t->value + 1);
                    merged->mergedFrom = 1;
                    
                    
                    insertTile(gm->grid, merged);
                    removeTile(gm->grid, t);
                    
                    updatePosition(t, fp.next);
                    
                    gm->score += (1 << merged->value);
                    
                    if((1 << merged->value) == TILE_2048){
                        gm->won = 1;
                    }
                    free(t);
                    free(next);
                } else {
                    moveTile(gm, t, fp.farthest);
                }
                
                if(!positionsEqual(p, t->position)){
                    moved = 1;
                }
            }
        }
    }
    
    if(moved){
        addRandomTile(gm);
        
        if(!movesAvailable(gm)){
            gm->over = 1;
        }
        
        actuate(gm);
    }
}

FarthestPosition findFarthestPosition(GameManager const * const gm, Position p, Vector const v){
    Position previous;
    do {
        previous = p;
        p = (Position){.x = previous.x + v.x,   .y = previous.y + v.y};
    } while(withinBounds(p) && !cellOccupied(gm->grid, p));
    
    return (FarthestPosition){.farthest = previous, .next = p};
}

uint8_t movesAvailable(GameManager const * const gm){
    return cellsAvailable(gm->grid) || tileMatchesAvailable(gm);
}

uint8_t tileMatchesAvailable(GameManager const * const gm){
    Tile* t;
    for(int x = 0; x < SIZE; x++){
        for(int y = 0; y < SIZE; y++){
            t = gm->grid->tiles[x][y];
            if(t){
                for(int dir = 0; dir < 4; dir++){
                    Vector v    = getVector(dir);
                    Position p  = {.x = x + v.x, .y = y + v.y};
                    Tile* other = cellContent(gm->grid, p);
                    if(other && other->value == t->value){
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}


// --- VECTOR FUNCTIONS --- //
Vector getVector(Direction d){
    switch(d){
    case dir_up:    return (Vector){.x = 0,  .y = -1};
    case dir_right: return (Vector){.x = 1,  .y = 0 };
    case dir_down:  return (Vector){.x = 0,  .y = 1 };
    case dir_left:  return (Vector){.x = -1, .y = 0 };
    default:        return (Vector){.x = 0,  .y = 0 };
    }
}


// --- ACTUATOR FUNCTIONS --- //
Actuator* new_Actuator(void){
    Actuator* a = malloc(sizeof(Actuator));
    memset(a, 0, sizeof(Actuator));

    a->actuate = actuateFrameBuffer;
    drawBackground();
    return a;
}

#define SLIDER_L_BBOX           69,  85,  54,  62
#define SLIDER_L_TEXT_CURSOR    71,  56
#define SLIDER_R_BBOX           111, 126, 54,  62
#define SLIDER_R_TEXT_CURSOR    113, 56

void drawBackground(void){
    fbuf_clear();
    for(uint8_t x = 0; x <= GRID_SIZE_X*4; x += GRID_SIZE_X){
        for(uint8_t y = 0; y <= GRID_SIZE_Y*4; y += GRID_SIZE_Y){
            fbuf_draw_rectangle(0, x, 0, y);
        }
    }
        
    fbuf_set_font(font8x8());
    fbuf_set_font_spacing(-2, 0);

    fbuf_set_cursor_to_pixel(GRID_SIZE_X*4 + 3, 0);
    fbuf_printf("Score");

    fbuf_set_cursor_to_pixel(GRID_SIZE_X*4 + 3, 20);
    fbuf_printf("Best");
        
    fbuf_draw_rectangle(SLIDER_L_BBOX);
    fbuf_set_cursor_to_pixel(SLIDER_L_TEXT_CURSOR);
    fbuf_set_font(font_2048());
    fbuf_draw_char(17);     // Quit

    fbuf_render();
}

void actuateFrameBuffer(Grid const * const g, ActuatorMetadata const am){
    for(int x = 0; x < SIZE; x++){
        for(int y = 0; y < SIZE; y++){
            fbuf_clear_area(x*GRID_SIZE_X+1, (x+1)*GRID_SIZE_X-1, y*GRID_SIZE_Y+1, (y+1)*GRID_SIZE_Y-1);
            if(g->tiles[x][y]){
                fbuf_set_font(font_2048());
                fbuf_set_cursor_to_pixel(x*GRID_SIZE_X+2, y*GRID_SIZE_Y+6);
                fbuf_draw_char(g->tiles[x][y]->value);
            }
        }
    }

    fbuf_set_font(font8x8());
    fbuf_set_font_spacing(-2, 0);

    fbuf_set_cursor_to_pixel(GRID_SIZE_X*SIZE + 2, 8);
    fbuf_printf("%6ld", am.score);

    fbuf_set_cursor_to_pixel(GRID_SIZE_X*SIZE + 2, 28);
    fbuf_printf("%6ld", am.bestScore);

    if(am.terminated){
        if(am.over){
            fbuf_set_font_spacing(0, 0);
            fbuf_set_cursor_to_pixel(2, 25);
            fbuf_printf("Game over!");
            
            fbuf_set_font_spacing(-1, 0);
            fbuf_set_cursor_to_pixel(11, 46);
            fbuf_printf("New game?");
        } else if(am.won){
            fbuf_set_font_spacing(0, 0);
            fbuf_set_cursor_to_pixel(9, 25);
            fbuf_printf("You win!");
            
            fbuf_set_font_spacing(-1, 0);
            fbuf_set_cursor_to_pixel(11, 46);
            fbuf_printf("Keep Playing?");            
        }
        fbuf_set_font(font_2048());
        fbuf_draw_rectangle(SLIDER_L_BBOX);
        fbuf_set_cursor_to_pixel(SLIDER_L_TEXT_CURSOR);
        fbuf_draw_char(15);     // No
        fbuf_draw_rectangle(SLIDER_R_BBOX);
        fbuf_set_cursor_to_pixel(SLIDER_R_TEXT_CURSOR);
        fbuf_draw_char(16);     // Yes
    }
    fbuf_render();
}


// --- MAIN / GAME --- //

void game_2048(){
    srand(TCNT3);
    GameManager_scoped* gm = new_GameManager();
    
    Direction   inputDirn       = dir_down;
    JOY_dir_t   joyDirn         = JOY_get_direction();
    JOY_dir_t   joyDirnPrev     = joyDirn;
    uint8_t     quit            = 0;

    fbuf_set_font(font8x8());
    fbuf_set_font_spacing(-2, 0);
    
    drawBackground();   // for some reason the text isn't drawn the first time. (WAT)
    actuate(gm);

    while(!quit){
        joyDirnPrev = joyDirn;
        joyDirn     = JOY_get_direction();
        if(joyDirn != joyDirnPrev && joyDirn != NEUTRAL){
            switch(joyDirn){
                case UP:        inputDirn = dir_up;     break;
                case RIGHT:     inputDirn = dir_right;  break;
                case DOWN:      inputDirn = dir_down;   break;
                case LEFT:      inputDirn = dir_left;   break;
                default: break;
            }
            move(gm, inputDirn);    
        }

        if(isGameTerminated(gm)){
            if(gm->over){
                if(SLI_get_left_button()){  // quit
                    fbuf_clear();
                    fbuf_render();
                    quit = 1;
                }
                if(SLI_get_right_button()){ // restart
                    fbuf_clear();
                    writeCurrentUserProfile(gm->userProfile);
                    delete_GameManager(&gm);
                    gm = new_GameManager();
                    drawBackground();
                    actuate(gm);
                }
            } else if(gm->won){
                if(SLI_get_left_button()){  // quit
                    fbuf_clear();
                    fbuf_render();
                    quit = 1;
                }
                if(SLI_get_right_button()){ // keep playing
                    gm->keepPlaying = 1;
                    fbuf_clear();
                    drawBackground();
                    actuate(gm);
                }
            }
        } else {
            if(SLI_get_left_button()){  // quit
                fbuf_clear();
                fbuf_render();
                quit = 1;
            }
        }
    }
    writeCurrentUserProfile(gm->userProfile);
    
    return;
}


void __attribute__((constructor)) game_2048_init(void){
    // Start the timer for seeding random numbers
    TCCR3B |= 1<<(CS30);
}    