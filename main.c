#include <stdio.h>
#include <stdlib.h>

#define SIZE_X 19
#define SIZE_Y 11
#define INVADERS_SIZE_Y 4
#define INVADERS_SIZE_X 11

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

void SpawnInvaders(Entity*** grid){
    for (int y = 0; y < INVADERS_SIZE_Y; ++y){
        for (int x = 0; x < INVADERS_SIZE_X; ++x){
            grid[y][x] = NewEntity('M', x, y);
        }
    }
}

void DrawGrid(Entity*** grid){
    for (int y = 0; y < SIZE_Y; ++y){
        for (int x = 0; x < SIZE_X; ++x){
            if (grid[y][x] == NULL){
                printf("   ");
            }
            else{
                printf(" %c ", grid[y][x]->skin);
            }
        }
        printf("\n");
    }
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

int main(){
    int running = 1;

    Entity*** GRID = InitializeGrid();
    Entity* PLAYER = GridEntity(GRID, 'A', 0, SIZE_Y - 1);
    Entity* BULLET = NULL;

    SpawnInvaders(GRID);

    char i;
    while (running){
        // Make current bullet move up; if the grid space that will be hit is an invader, delete the bullet and the invader
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
    }

    return 0;
}
