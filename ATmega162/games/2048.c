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

#define TILE_2048       16  // The tile value needed to win (2048 by default)

typedef struct Position         Position;
typedef struct FarthestPosition FarthestPosition;
typedef struct Tile             Tile;
typedef struct Grid             Grid;
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
    Tile* mergingTiles[SIZE][SIZE];
};


/** The GameManager is the core of the game
*/
struct GameManager {
    UserProfile*        userProfile;
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
    Up/down is inverted, because grid display is most commonly done from top to bottom
*/
typedef struct Vector Vector;
struct Vector {
    int8_t      x;
    int8_t      y;
};



// --- POSITION FUNCTIONS --- //
static inline uint8_t withinBounds(Position const p){
    return 
        p.x     >=  0       &&
        p.y     >=  0       &&
        p.x     <   SIZE    &&
        p.y     <   SIZE    ;
}

static inline uint8_t positionsEqual(Position const first, Position const second){
    return first.x == second.x  &&  first.y == second.y;
}

static inline int8_t traversal(int8_t dir, int8_t dist){
    return dir == 1 ? SIZE-1-dist : dist;
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


// --- TILE FUNCTIONS --- //
Tile* new_Tile(Position const p, uint8_t value){
    Tile* t = malloc(sizeof(Tile));
    memset(t, 0, sizeof(Tile));
    t->position = p;
    t->value    = value;
    return t;
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

void insertTile(Grid* const g, Tile* const t){
    g->tiles[t->position.x][t->position.y] = t;
}

void moveTile(Grid* const g, Tile* const t, Position const p){
    g->tiles[t->position.x][t->position.y] = NULL;
    g->tiles[p.x][p.y] = t;
    updatePosition(t, p);
}

void setMerging(Grid* const g, Tile const * const t){
    g->mergingTiles[t->position.x][t->position.y] =
        g->tiles[t->position.x][t->position.y];

    g->tiles[t->position.x][t->position.y] = NULL;
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

uint8_t cellsAvailable(Grid const * const g){
    for(int8_t x = 0; x < SIZE; x++){
        for(int8_t y = 0; y < SIZE; y++){
            if(!g->tiles[x][y]){
                return 1;
            }
        }
    }
    return 0;
}

uint8_t tileMatchesAvailable(Grid const * const g){
    Tile* t;
    for(int8_t x = 0; x < SIZE; x++){
        for(int8_t y = 0; y < SIZE; y++){
            t = g->tiles[x][y];
            if(t){
                for(int8_t dir = 0; dir < 4; dir++){
                    Vector v    = getVector(dir);
                    Position p  = {.x = x + v.x, .y = y + v.y};
                    Tile* other = cellContent(g, p);
                    if(other && other->value == t->value){
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

uint8_t movesAvailable(Grid const * const g){
    return cellsAvailable(g) || tileMatchesAvailable(g);
}

FarthestPosition findFarthestPosition(Grid const * const g, Position p, Vector const v){
    Position previous;
    do {
        previous = p;
        p = (Position){.x = previous.x + v.x,   .y = previous.y + v.y};
    } while(withinBounds(p) && !cellOccupied(g, p));
    
    return (FarthestPosition){.farthest = previous, .next = p};
}

Position randomAvailablePosition(Grid const * const g){
    Position p;
    do {
        p.x = rand() % SIZE;
        p.y = rand() % SIZE;
    } while(cellOccupied(g, p));
    return p;
}

void addRandomTile(Grid* const g){
    Tile* t = new_Tile(randomAvailablePosition(g), ((rand() % 10) < 9) ? 1 : 2);
    insertTile(g, t);
}

void prepareTiles(Grid* const g){
    for(int8_t x = 0; x < SIZE; x++){
        for(int8_t y = 0; y < SIZE; y++){
            if(g->tiles[x][y]){
                g->tiles[x][y]->mergedFrom = 0;
                g->tiles[x][y]->previousPosition = g->tiles[x][y]->position;
            }
            if(g->mergingTiles[x][y]){
                free(g->mergingTiles[x][y]);
                g->mergingTiles[x][y] = NULL;
            }
        }
    }
}


// --- USERPROFILE FUNCTIONS --- //
uint32_t getBestScore(UserProfile const * const p){
    return p->game_2048.bestScore;
}

void setBestScore(UserProfile* const p, uint32_t const score){
    p->game_2048.bestScore = score;
}

void saveGameState(UserProfile* const p, Grid const * const g){
    for(int8_t x = 0; x < SIZE; x++){
        for(int8_t y = 0; y < SIZE; y++){
            if(g->tiles[x][y]){
                p->game_2048.grid[x][y] = g->tiles[x][y]->value;
            } else {
                p->game_2048.grid[x][y] = 0;
            }
        }
    }
}

void clearGameState(UserProfile* const p){
    for(int8_t x = 0; x < SIZE; x++){
        for(int8_t y = 0; y < SIZE; y++){
            p->game_2048.grid[x][y] = 0;
        }
    }
}


// --- GAME MANAGER FUNCTIONS --- //

void addStartTiles(GameManager* const gm);
void actuate(GameManager* const gm);
void displayGameState(GameManager const * const gm);

GameManager* new_GameManager(){
    GameManager* gm = malloc(sizeof(GameManager));
    memset(gm, 0, sizeof(GameManager));
    
    gm->grid            = new_Grid();
    gm->numStartTiles   = NUM_START_TILES;
    gm->userProfile     = malloc(sizeof(UserProfile));
    *gm->userProfile    = getCurrentUserProfile();
    
    uint8_t foundStoredGame = 0;

    for(int8_t x = 0; x < SIZE; x++){
        for(int8_t y = 0; y < SIZE; y++){
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
    for(int8_t x = 0; x < SIZE; x++){
        for(int8_t y = 0; y < SIZE; y++){
            if((*gm)->grid->tiles[x][y]){
                free((*gm)->grid->tiles[x][y]);
                free((*gm)->grid->mergingTiles[x][y]);
            }
        }
    }
    free((*gm)->grid);
    free((*gm)->userProfile);
    free(*gm);
    return;
}

void addStartTiles(GameManager* const gm){
    for(uint8_t i = 0; i < gm->numStartTiles; i++){
        addRandomTile(gm->grid);
    }
}

uint8_t isGameTerminated(GameManager const * const gm){
    return gm->over || (gm->won && !gm->keepPlaying);
}

void move(GameManager* const gm, Direction const d){
    if(isGameTerminated(gm)) return;
    
    Vector      v       = getVector(d);
    uint8_t     moved   = 0;
    
    prepareTiles(gm->grid);
    
    
    for(int8_t ix = 0; ix < SIZE; ix++){
        for(int8_t iy = 0; iy < SIZE; iy++){
            Position p = (Position){
                .x = traversal(v.x, ix),
                .y = traversal(v.y, iy),
            };
            Tile* t = cellContent(gm->grid, p);
            
            if(t){
                FarthestPosition    fp      = findFarthestPosition(gm->grid, p, v);
                Tile*               next    = cellContent(gm->grid, fp.next);
                
                if( next  &&  
                    next->value == t->value  &&  
                    !next->mergedFrom
                ){
                    Tile* merged = new_Tile(fp.next, t->value + 1);
                    merged->mergedFrom = 1;
                    
                    
                    setMerging(gm->grid, t);
                    setMerging(gm->grid, next);
                    updatePosition(t, fp.next);
                    insertTile(gm->grid, merged);                    
                    
                    gm->score += (1 << merged->value);
                    
                    if((1 << merged->value) == TILE_2048){
                        gm->won = 1;
                    }
                } else {
                    moveTile(gm->grid, t, fp.farthest);
                }
                
                if(!positionsEqual(p, t->position)){
                    moved = 1;
                }
            }
        }
    }
    
    if(moved){
        addRandomTile(gm->grid);
        
        if(!movesAvailable(gm->grid)){
            gm->over = 1;
        }
        
        actuate(gm);
    }
}

void actuate(GameManager* const gm){
    
    if(getBestScore(gm->userProfile) < gm->score){
        setBestScore(gm->userProfile, gm->score);
    }
    
    
    if(isGameTerminated(gm)){
        clearGameState(gm->userProfile);
    } else {
        saveGameState(gm->userProfile, gm->grid);
    }

    displayGameState(gm);
}

#define GRID_SIZE_X     16
#define GRID_SIZE_Y     15
#define SLIDER_L_BBOX           69,  85,  54,  62
#define SLIDER_L_TEXT_CURSOR    71,  56
#define SLIDER_R_BBOX           111, 126, 54,  62
#define SLIDER_R_TEXT_CURSOR    113, 56

void drawBackground(void){
    fbuf_clear();
    for(uint8_t x = 0; x <= GRID_SIZE_X*SIZE; x += GRID_SIZE_X){
        for(uint8_t y = 0; y <= GRID_SIZE_Y*SIZE; y += GRID_SIZE_Y){
            fbuf_draw_rectangle(0, x, 0, y);
        }
    }
        
    fbuf_set_font(font8x8());
    fbuf_set_font_spacing(-2, 0);

    fbuf_set_cursor_to_pixel(GRID_SIZE_X*SIZE + 3, 0);
    fbuf_printf("Score");

    fbuf_set_cursor_to_pixel(GRID_SIZE_X*SIZE + 3, 20);
    fbuf_printf("Best");
        
    fbuf_draw_rectangle(SLIDER_L_BBOX);
    fbuf_set_cursor_to_pixel(SLIDER_L_TEXT_CURSOR);
    fbuf_set_font(font_2048());
    fbuf_draw_char(FONT2048_CHARNUM_QUIT);

    fbuf_render();
}

void displayGameState(GameManager const * const gm){
    fbuf_set_font(font_2048());
    for(int8_t x = 0; x < SIZE; x++){
        for(int8_t y = 0; y < SIZE; y++){
            fbuf_clear_area(x*GRID_SIZE_X+1, (x+1)*GRID_SIZE_X-1, y*GRID_SIZE_Y+1, (y+1)*GRID_SIZE_Y-1);
            if(gm->grid->tiles[x][y]){
                fbuf_set_cursor_to_pixel(x*GRID_SIZE_X+2, y*GRID_SIZE_Y+6);
                fbuf_draw_char(gm->grid->tiles[x][y]->value);
            }
        }
    }

    /*
    Animations:
        Animation order:
            Movement                (~5 frames)
            New tiles from merge    (~4 frames (1 overlap from movement))
            New random tile         (~4 frames)

    Outline:
        for(int y = 0; y < SIZE; y++){
            for(int x = 0; x < SIZE; x++){
                Tile* pt = g->mergingTiles[x][y];
                if(pt){
                    printf("Move-m (%d, %d) to (%d, %d)\n",
                        pt->previousPosition.x, pt->previousPosition.y, pt->position.x, pt->position.y);
                }
                Tile* t = g->tiles[x][y];
                if(t){
                    if(positionsEqual(t->previousPosition, (Position){-1, -1})){
                        printf("New at (%d, %d) (value: %d) (merged: %d)\n", x, y, 1 << t->value, t->merged);
                    } else if(!positionsEqual(t->previousPosition, t->position)){
                        printf("Move   (%d, %d) to (%d, %d)\n",
                            t->previousPosition.x, t->previousPosition.y, t->position.x, t->position.y);
                    }
                }
            }
        }
    */

    fbuf_set_font(font8x8());
    fbuf_set_font_spacing(-2, 0);

    fbuf_set_cursor_to_pixel(GRID_SIZE_X*SIZE + 2, 8);
    fbuf_printf("%6ld", gm->score);

    fbuf_set_cursor_to_pixel(GRID_SIZE_X*SIZE + 2, 28);
    fbuf_printf("%6ld", getBestScore(gm->userProfile));

    if(isGameTerminated(gm)){
        if(gm->over){
            fbuf_set_font_spacing(0, 0);
            fbuf_set_cursor_to_pixel(2, 25);
            fbuf_printf("Game over!");
            
            fbuf_set_font_spacing(-1, 0);
            fbuf_set_cursor_to_pixel(11, 46);
            fbuf_printf("New game?");
        } else if(gm->won){
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
        fbuf_draw_char(FONT2048_CHARNUM_NO);
        fbuf_draw_rectangle(SLIDER_R_BBOX);
        fbuf_set_cursor_to_pixel(SLIDER_R_TEXT_CURSOR);
        fbuf_draw_char(FONT2048_CHARNUM_YES);
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

    fbuf_set_font(font8x8());
    fbuf_set_font_spacing(-2, 0);
    
    drawBackground();
    actuate(gm);

    while(1){
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
                    goto quit;
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
                    goto quit;
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
                goto quit;
            }
        }
    }
quit:
    fbuf_clear();
    fbuf_render();
    writeCurrentUserProfile(gm->userProfile);
    
    return;
}


void __attribute__((constructor)) game_2048_init(void){
    // Start the timer for seeding random numbers
    TCCR3B |= 1<<(CS30);
}    