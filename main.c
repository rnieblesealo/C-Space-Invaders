// ASCII Space Invaders in C by Rafael Niebles.
// Do not distribute! <3

/* BUGS/WEIRDNESS:
    - Moving an entity outside the display's X bounds works fine, but for Y bound, a segfault occurs
*/

#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT_LENGTH 1024

#define X_SIZE 10
#define Y_SIZE 18

#define K_SHOOT 'c'
#define K_MOVE_UP 'w'
#define K_MOVE_DOWN 's'
#define K_MOVE_LEFT 'a'
#define K_MOVE_RIGHT 'd'

#define X_INVADER_COUNT 6
#define Y_INVADER_COUNT 4

#define SKIN_PLAYER 'A'
#define SKIN_INVADER 'M'

enum Direction{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

typedef struct Vector2{
    int x;
    int y;
} Vector2;

typedef struct Entity{
    Vector2 position;
    char skin;
    int alive;
} Entity;

// Constructors
Vector2 new_Vector2(int x, int y);
Entity new_Entity(Vector2 position, char skin);

// Display
char** InitDisplay();
void ClearDisplay(char** display);
void Display(char** display);
void DrawEntity(char** display, Entity e);

// Input
char GetCommand();

// Game
Entity** InitInvaders();
void DrawInvaders(char** display, Entity** invaders);
void MoveInvaders(Entity** invaders, enum Direction dir);

int main(){
    char** display = InitDisplay();

    Entity** invaders = InitInvaders();

    int quit = 0;
    while (!quit){
        ClearDisplay(display);
        
        // Add entity drawing code here!
        DrawInvaders(display, invaders);
        
        Display(display);
        
        // Obtain input, match to keycode, if no match, advance game!
        char command = GetCommand();
        switch (command){
            case K_MOVE_UP:
                MoveInvaders(invaders, UP);
                break;
            case K_MOVE_DOWN:
                MoveInvaders(invaders, DOWN);
                break;
            case K_MOVE_LEFT:
                MoveInvaders(invaders, LEFT);
                break;
            case K_MOVE_RIGHT:
                MoveInvaders(invaders, RIGHT);
                break;
            case K_SHOOT:
                // Add shoot
                break;
            default:
                break;
        }

        // Add game update code here!
    }

    Display(display);

    return 0;
}

Vector2 new_Vector2(int x, int y){
    Vector2 n;

    n.x = x;
    n.y = y;

    return n;
}

Entity new_Entity(Vector2 position, char skin){
    Entity e;

    e.position.x = position.x;
    e.position.y = position.y;

    e.skin = skin;
    
    // All entities are alive by default
    e.alive = 1;

    return e;
}

char** InitDisplay(){
    // Make rows and columns of display into a 2D char array, initialize all chars to ' '
    char** rows = (char**)malloc(Y_SIZE * sizeof(char*));

    for (int y = 0; y < Y_SIZE; ++y){
        rows[y] = (char*)malloc(X_SIZE * sizeof(char));
        
        for (int x = 0; x < X_SIZE; ++x){
            rows[y][x] = ' ';
        }
    }

    return rows;
}

void ClearDisplay(char** display){
    // Clear console/terminal
    // NOTE This is not portable and works only in UNIX-like environments!
    system("clear");
    
    // Clear display by replacing all characters with whitespace
    for (int y = 0; y < Y_SIZE; ++y){
        for (int x = 0; x < X_SIZE; ++x){
            display[y][x] = ' ';
        }
    }
}

void Display(char** display){
    // Show the display as-is
    for (int y = 0; y < Y_SIZE; ++y){
        putchar('[');
        for (int x = 0; x < X_SIZE; ++x){
            printf(" %c ", display[y][x]);
        }
        puts("]");
    }
}

void DrawEntity(char** display, Entity e){
    // Add an entity to the display
    display[e.position.y][e.position.x] = e.skin;
}

char GetCommand(){
    char input[MAX_INPUT_LENGTH];
    char firstInput = 0;
    
    // Use fgets to clear input buffer
    fgets(input, MAX_INPUT_LENGTH, stdin);
    firstInput = input[0];

    // Make first input lowercase if it is a letter so program isn't case-sensitive
    if (firstInput >= 'A' && firstInput <= 'Z'){
        firstInput += 32;
    }

    return firstInput;
}

Entity** InitInvaders(){
    // Allocate invader row
    Entity** rows = (Entity**)malloc(Y_INVADER_COUNT * sizeof(Entity*));
    
    // Allocate columns
    for (int y = 0; y < Y_INVADER_COUNT; ++y){
        rows[y] = (Entity*)malloc(X_INVADER_COUNT * sizeof(Entity));
        
        for (int x = 0; x < X_INVADER_COUNT; ++x){
            // Make invaders!
            rows[y][x] = new_Entity(new_Vector2(x, y), SKIN_INVADER);
        }
    }

    return rows;
}

void DrawInvaders(char** display, Entity** invaders){
    // Draw invaders that are alive
    for (int y = 0; y < Y_INVADER_COUNT; ++y){
        for (int x = 0; x < X_INVADER_COUNT; ++x){
            if (invaders[y][x].alive){
                DrawEntity(display, invaders[y][x]);
            }
        }
    }
}

void MoveInvaders(Entity** invaders, enum Direction dir){
    // Determine direction
    int dx;
    int dy;
    switch (dir){
        case UP:
            dx = 0;
            dy = -1;
            break;
        case DOWN:
            dx = 0;
            dy = 1;
            break;
        case LEFT:
            dx = -1;
            dy = 0;
            break;
        case RIGHT:
            dx = 1;
            dy = 0;
            break;
        default:
            break;
    }
    
    // Move invaders that are alive
    for (int y = 0; y < Y_INVADER_COUNT; ++y){
        for (int x = 0; x < X_INVADER_COUNT; ++x){
            if (invaders[y][x].alive){
                invaders[y][x].position.x += dx;
                invaders[y][x].position.y += dy;
            }
        }
    }
}