#include <stdio.h>
#include <stdlib.h>

#define SIZE_X 19
#define SIZE_Y 11
#define INVADERS_SIZE_Y 4
#define INVADERS_SIZE_X 11

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct Entity{
    char skin;
    int* pos;
} Entity;

Entity* NewEntity(char skin, int pos_x, int pos_y){
    Entity* n = (Entity*)malloc(sizeof(Entity));
    
    n->skin = skin;
    
    n->pos = (int*)malloc(sizeof(int) * 2);
    n->pos[0] = pos_x;
    n->pos[1] = pos_y;

    return n;
}

Entity* GridEntity(Entity*** grid, char skin, int pos_x, int pos_y){
    Entity* n = NewEntity(skin, pos_x, pos_y);

    n->skin = skin;
    
    n->pos = (int*)malloc(sizeof(int) * 2);
    n->pos[0] = pos_x;
    n->pos[1] = pos_y;

    // Add to grid before returning
    grid[pos_y][pos_x] = n;
    return n;
}

Entity*** InitializeGrid(){
    Entity*** grid = (Entity***)malloc(SIZE_Y * sizeof(Entity**));
    for (int y = 0; y < SIZE_Y; ++y){
        Entity** row = (Entity**)malloc(SIZE_X * sizeof(Entity*));
        for (int x = 0; x < SIZE_X; ++x){
            row[x] = NULL;
        }
        grid[y] = row;
    }
    return grid;
}   

void MoveEntity(Entity*** grid, int pos_x, int pos_y, int dx, int dy){
    // First check if coordinates are in bounds
    if ((pos_x < 0 || pos_x >= SIZE_X) || (pos_y < 0 || pos_y >= SIZE_Y) || (pos_x + dx < 0 || pos_x + dx >= SIZE_X) || (pos_y + dy < 0 || pos_y + dy >= SIZE_Y)){
        return;
    }
    
    // Obtain temp, write values to pointed to entity if there is one at all
    Entity* temp = grid[pos_y + dy][pos_x + dx];
    
    if (temp != NULL){
        temp->pos[0] = pos_x;
        temp->pos[1] = pos_y;
    }
    
    // Write values to target node if it is not null as well
    if (grid[pos_y][pos_x] != NULL){
        grid[pos_y][pos_x]->pos[0] = pos_x + dx;
        grid[pos_y][pos_x]->pos[1] = pos_y + dy;
    }
    
    // Perform swap
    grid[pos_y + dy][pos_x + dx] = grid[pos_y][pos_x];
    grid[pos_y][pos_x] = temp;
}

void SpawnInvaders(Entity*** grid){
    for (int y = 0; y < INVADERS_SIZE_Y; ++y){
        for (int x = 0; x < INVADERS_SIZE_X; ++x){
            grid[y][x] = NewEntity('M', x, y);
        }
    }
}

void MoveEntitiesRight(Entity*** grid, int x, int y, int dx, int dy){
	for (int i = 0; i < INVADERS_SIZE_Y; ++i){
		for (int j = x + INVADERS_SIZE_X - 1; j > x - 1; --j){
		    MoveEntity(grid, j, i, dx, dy);
		}
	}
}

void MoveEntitiesLeft(Entity*** grid, int x, int y, int dx, int dy){
    for (int i = y; i < y + INVADERS_SIZE_Y; ++i){
        for (int j = x; j < x + INVADERS_SIZE_X; ++j){
            MoveEntity(grid, j, i, dx, dy);
        }
    }
}

void DrawGrid(Entity*** grid){
    for (int y = 0; y < SIZE_Y; ++y){
        printf(ANSI_COLOR_YELLOW "# " ANSI_COLOR_RESET);
        for (int x = 0; x < SIZE_X; ++x){
            if (grid[y][x] == NULL){
                printf("   ");
            }
            else{
                printf(" %c ", grid[y][x]->skin);
            }
        }
        printf(ANSI_COLOR_YELLOW " #\n" ANSI_COLOR_RESET);
    }
}

int main(){
    int running = 1;
    int time = 0;
    int moveTime = 2;
    int nextMoveTime = moveTime;

    int dx = 1; // How much to move invaders by

    // Function pointer to move function
    void (*moveFunction)(Entity*** grid, int x, int y, int dx, int dy);

    Entity*** GRID = InitializeGrid();
    Entity* PLAYER = GridEntity(GRID, 'A', 0, SIZE_Y - 1);
    Entity* BULLET = NULL;

    SpawnInvaders(GRID);

    // Store topleft of invaders in memory so that we can easily modify where all invaders go
    int invadersPosition[2] = {0, 0};

    char i;
    while (running){
        // Make current bullet move up; if the grid space that will be hit is an invader, delete the bullet and the invader
        // ! Add check for when position of bullet target is out of bounds
        if (BULLET != NULL){
            if (GRID[BULLET->pos[1] - 1][BULLET->pos[0]] != NULL){
                // Set memory alias
                Entity* dest = GRID[BULLET->pos[1] - 1][BULLET->pos[0]];

                // Remove destroyed and bullet from grid
                GRID[BULLET->pos[1] - 1][BULLET->pos[0]] = NULL;
                GRID[BULLET->pos[1]][BULLET->pos[0]] = NULL;
                
                // Free bullet and destroyed from memory
                free(dest);
                free(BULLET);

                // Bullet is now null
                BULLET = NULL;
            }
            else{
                MoveEntity(GRID, BULLET->pos[0], BULLET->pos[1], 0, -1);
            }
        }
                
        // Move invaders if time
        if (time > nextMoveTime){
            if (invadersPosition[0] == (SIZE_X - INVADERS_SIZE_X - 1)){
                dx = -1;
                moveFunction = &MoveEntitiesLeft;
            }

            else if (invadersPosition[0] == 0){
                dx = 1;
                moveFunction = &MoveEntitiesRight;
            }
            
            moveFunction(GRID, invadersPosition[0], invadersPosition[1], dx, 0);
            invadersPosition[0] += dx;

            nextMoveTime = time + moveTime;
        }

        DrawGrid(GRID);
        
        scanf("%c", &i);
        switch (i){
            case 'a':
                MoveEntity(GRID, PLAYER->pos[0], PLAYER->pos[1], -1, 0);
                break;
            case 'd':
                MoveEntity(GRID, PLAYER->pos[0], PLAYER->pos[1], 1, 0);
                break;
            case 's':
                // Fire!
                if (BULLET == NULL){
                    BULLET = GridEntity(GRID, '^', PLAYER->pos[0], PLAYER->pos[1] - 1);
                }
                break;
            default:
                break;
        }
        system("clear");
        time++;
    }

    return 0;
}
