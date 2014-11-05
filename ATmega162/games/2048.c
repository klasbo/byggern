#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <avr/io.h>

#define SIZE 4
#define NUM_START_TILES 2

uint8_t traversal_asc[4] = {0, 1, 2, 3};
uint8_t traversal_dsc[4] = {3, 2, 1, 0};

typedef struct Position         Position;
typedef struct FarthestPosition FarthestPosition;
typedef struct Tile             Tile;
typedef struct Grid             Grid;
typedef struct StorageManager   StorageManager;
typedef struct Actuator         Actuator;
typedef struct ActuatorMetadata ActuatorMetadata;
#define GameManager_scoped __attribute__((cleanup(delete_GameManager))) GameManager
typedef struct GameManager      GameManager;


struct Position {
    int8_t      x;
    int8_t      y;
};

struct FarthestPosition {
    Position    farthest;
    Position    next;
};


// A tile is placed on the grid, and always has a value
// The actual value is 2^value, to conserve space
struct Tile {
    uint8_t     value;
    Position    position;
    Position    previousPosition;
//    TilePair    mergedFrom;
    struct TilePair {
        Tile*   first;
        Tile*   second;
    } mergedFrom;
};
/*
struct TilePair {
    Tile*       first;
    Tile*       second;
};*/

/** The grid consists of 4x4 cells
*   Each cell may containg a single Tile (may be NULL)
*/
struct Grid {
    Tile* tiles[SIZE][SIZE];
};


struct StorageManager {
    uint32_t    (*getBestScore)(void);
    void        (*setBestScore)(uint32_t score);
    //void        clearGameState(void);
    //void        setGameState(void* data, uint16_t size);
};

struct Actuator {
    void        (*actuate)(Grid*, ActuatorMetadata);
};

struct ActuatorMetadata {
    uint32_t    score;
    uint32_t    bestScore;
    uint8_t     over;
    uint8_t     won;
    uint8_t     terminated;
};

struct GameManager {
    //InputManager*       inputManager;
    StorageManager*     storageManager;
    Actuator*           actuator;
    Grid*               grid;
    
    uint8_t             numStartTiles;
    uint32_t            score;
    uint8_t             over;
    uint8_t             won;
    uint8_t             keepPlaying;
};

typedef enum {
    dir_up,
    dir_right,
    dir_down,
    dir_left
} Direction;

typedef struct Vector Vector;
struct Vector {
    int8_t      x;
    int8_t      y;
};

// --- POSITION FUNCTIONS --- //
uint8_t withinBounds(Position p);
uint8_t positionsEqual(Position first, Position second);
// --- TILE FUNCTIONS --- //
Tile* new_Tile(Position p);
void savePosition(Tile* t);
void updatePosition(Tile* t, Position p);
// --- GRID FUNCTIONS --- //
Grid* new_Grid();
uint8_t cellsAvailable(Grid* g);
Tile* cellContent(Grid* g, Position p);
uint8_t cellOccupied(Grid* g, Position p);
void insertTile(Grid* g, Tile* t);
void removeTile(Grid* g, Tile* t);
void eachCell(Grid* g, void fcn(Position p, Tile* t));
// --- GAME MANAGER FUNCTIONS --- //
GameManager* new_GameManager();
void delete_GameManager(GameManager** gm);
uint8_t isGameTerminated(GameManager* gm);
void addStartTiles(GameManager* gm);
void addRandomTile(GameManager* gm);
Position randomAvailablePosition(GameManager* gm);
void actuate(GameManager* gm);
void prepareTiles(GameManager* gm);
void moveTile(GameManager* gm, Tile* t, Position p);
void move(GameManager* gm, Direction d);
FarthestPosition findFarthestPosition(GameManager* gm, Position p, Vector v);
uint8_t movesAvailable(GameManager* gm);
uint8_t tileMatchesAvailable(GameManager* gm);
// --- VECTOR FUNCTIONS --- //
Vector getVector(Direction d);
// --- MISC --- //
void printGrid(Grid* g);

// --- POSITION FUNCTIONS --- //
uint8_t withinBounds(Position p){
    return 
        p.x     >=  0       &&
        p.y     >=  0       &&
        p.x     <   SIZE    &&
        p.y     <   SIZE    ;
}

uint8_t positionsEqual(Position first, Position second){
    return first.x == second.x  &&  first.y == second.y;
}


// --- TILE FUNCTIONS --- //
Tile* new_Tile(Position p){
    Tile* t = malloc(sizeof(Tile));
    memset(t, 0, sizeof(Tile));
    t->position = p;
    t->value    = 1;
    return t;
}

void savePosition(Tile* t){
    t->previousPosition = t->position;
}

void updatePosition(Tile* t, Position p){
    t->position = p;
}

// --- GRID FUNCTIONS --- //
Grid* new_Grid(){
    Grid* g = malloc(sizeof(Grid));
    memset(g, 0, sizeof(Grid));
    return g;    
}

uint8_t cellsAvailable(Grid* g){
    for(int x = 0; x < SIZE; x++){
        for(int y = 0; y < SIZE; y++){
            if(!g->tiles[x][y]){
                return 1;
            }
        }
    }
    return 0;
}

Tile* cellContent(Grid* g, Position p){
    if(withinBounds(p)){
        return g->tiles[p.x][p.y];
    } else {
        return NULL;
    }
}

uint8_t cellOccupied(Grid* g, Position p){
    return !!g->tiles[p.x][p.y];
}

void insertTile(Grid* g, Tile* t){
    //printf("Inserting tile at (%d, %d)\n", t->position.x, t->position.y);
    g->tiles[t->position.x][t->position.y] = t;
}

void removeTile(Grid* g, Tile* t){
    g->tiles[t->position.x][t->position.y] = NULL;
    //free(t);
}

void eachCell(Grid* g, void fcn(Position p, Tile* t)){
    for(int x = 0; x < SIZE; x++){
        for(int y = 0; y < SIZE; y++){
            fcn((Position){.x = x, .y = y}, g->tiles[x][y]);
        }
    }
}

// --- GAME MANAGER FUNCTIONS --- //
GameManager* new_GameManager(){
    GameManager* gm = malloc(sizeof(GameManager));
    memset(gm, 0, sizeof(GameManager));
    
    gm->grid = new_Grid();
    gm->numStartTiles = NUM_START_TILES;
    
    addStartTiles(gm);
    
    return gm;
}


void delete_GameManager(GameManager** gm){
    for(int x = 0; x < SIZE; x++){
        for(int y = 0; y < SIZE; y++){
            if((*gm)->grid->tiles[x][y]){
                free((*gm)->grid->tiles[x][y]->mergedFrom.first);
                free((*gm)->grid->tiles[x][y]->mergedFrom.second);
                free((*gm)->grid->tiles[x][y]);
            }
        }
    }
    free((*gm)->grid);
    free(*gm);
    return;
}

uint8_t isGameTerminated(GameManager* gm){
    return gm->over || (gm->won && gm->keepPlaying);
}

void addStartTiles(GameManager* gm){
    for(uint8_t i = 0; i < gm->numStartTiles; i++){
        addRandomTile(gm);
    }
}

void addRandomTile(GameManager* gm){
    Tile* t = new_Tile(randomAvailablePosition(gm));
    insertTile(gm->grid, t);
}

Position randomAvailablePosition(GameManager* gm){
    Position p;
    do {
        p.x = rand() % SIZE;
        p.y = rand() % SIZE;
    } while(cellOccupied(gm->grid, p));
    return p;
}



void actuate(GameManager* gm){
    if(gm->storageManager->getBestScore() < gm->score){
        gm->storageManager->setBestScore(gm->score);
    }
    /*
    if(gm->over){
        gm->storageManager->clearGameState();
    } else {
        gm->storageManager->setGameState(serialize(gm));
    }
    */
    
    gm->actuator->actuate(
        gm->grid,
        (ActuatorMetadata){
            .score      = gm->score,
            .over       = gm->over,
            .won        = gm->won,
            .bestScore  = gm->storageManager->getBestScore(),
            .terminated = isGameTerminated(gm),
        }
    );
}

void prepareTiles(GameManager* gm){
    for(int x = 0; x < SIZE; x++){
        for(int y = 0; y < SIZE; y++){
            if(gm->grid->tiles[x][y]){
                free(gm->grid->tiles[x][y]->mergedFrom.first);
                gm->grid->tiles[x][y]->mergedFrom.first  = NULL;
                free(gm->grid->tiles[x][y]->mergedFrom.second);
                gm->grid->tiles[x][y]->mergedFrom.second = NULL;
                savePosition(gm->grid->tiles[x][y]);
            }
        }
    }
}

void moveTile(GameManager* gm, Tile* t, Position p){
    gm->grid->tiles[t->position.x][t->position.y] = NULL;
    gm->grid->tiles[p.x][p.y] = t;
    updatePosition(t, p);
}

void move(GameManager* gm, Direction d){
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
                    !next->mergedFrom.first && !next->mergedFrom.second
                ){
                    Tile* merged = new_Tile(fp.next);
                    merged->value = t->value + 1;
                    merged->mergedFrom.first  = t;
                    merged->mergedFrom.second = next;
                    
                    
                    insertTile(gm->grid, merged);
                    removeTile(gm->grid, t);
                    
                    updatePosition(t, fp.next);
                    
                    gm->score += (1 << merged->value);
                    
                    if((1 << merged->value) == 2048){
                        gm->won = 1;
                    }
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
        
        //gm->actuator->actuate();
    }
}

FarthestPosition findFarthestPosition(GameManager* gm, Position p, Vector v){
    Position previous;
    do {
        previous = p;
        p = (Position){.x = previous.x + v.x,   .y = previous.y + v.y};
    } while(withinBounds(p) && !cellOccupied(gm->grid, p));
    
    return (FarthestPosition){.farthest = previous, .next = p};
}

uint8_t movesAvailable(GameManager* gm){
    return cellsAvailable(gm->grid) || tileMatchesAvailable(gm);
}

uint8_t tileMatchesAvailable(GameManager* gm){
    Tile* t;
    for(int x = 0; x < SIZE; x++){
        for(int y = 0; y < SIZE; y++){
            t = gm->grid->tiles[x][y];
            if(t){
                for(int dir = 0; dir < 4; dir++){ // foreach(Direction)
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
    case dir_up:    return (Vector){.y = 0,  .x = -1};
    case dir_right: return (Vector){.y = 1,  .x = 0 };
    case dir_down:  return (Vector){.y = 0,  .x = 1 };
    case dir_left:  return (Vector){.y = -1, .x = 0 };
    default:        return (Vector){.y = 0,  .x = 0 };
    }
}


// --- MISC --- //
void printGrid(Grid* g){
    printf("+----+----+----+----+\n");
    for(int x = 0; x < SIZE; x++){
        printf("|");
        for(int y = 0; y < SIZE; y++){
            if(g->tiles[x][y]){
                printf("%4d|", 1 << g->tiles[x][y]->value);
            } else {
                printf("    |");
            }
        }
        printf("\n");
    }
    printf("+----+----+----+----+\n");
}



int game_2048(){
    srand(TCNT3);
    GameManager_scoped* gm = new_GameManager();
    
    char c;
    Direction d = dir_down;
        
    while(1){
        printGrid(gm->grid);
        scanf("\n%c", &c);
        printf("received %c\n", c);
        switch(c){
            case 'w': d = dir_up;       break;
            case 'd': d = dir_right;    break;
            case 's': d = dir_down;     break;
            case 'a': d = dir_left;     break;
            case 'q': return 0;
            default: break;
        }
        if(strchr("wasd", c) != NULL){
            move(gm, d);
        }
    }
    
    return 0;
}