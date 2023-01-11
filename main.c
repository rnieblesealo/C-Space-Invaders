// ASCII Space Invaders in C by Rafael Niebles.
// Do not distribute! <3

/* BUGS/WEIRDNESS:
   - Moving an entity outside the display's X bounds works fine, but for Y bound, a segfault occurs
*/

#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT_LENGTH 1024

#define B_GREEN "\e[1;32m"
#define B_WHITE "\e[1;37m"
#define C_RESET "\e[0m"

#define X_SIZE 14
#define Y_SIZE 11

#define K_SHOOT 'c'
#define K_MOVE_UP 'w'
#define K_MOVE_DOWN 's'
#define K_MOVE_LEFT 'a'
#define K_MOVE_RIGHT 'd'

#define X_INVADER_COUNT 8
#define Y_INVADER_COUNT 4

#define PLAYER_ROW 10
#define WALL_ROW 7

#define SKIN_PLAYER 'A'
#define SKIN_PLAYER_BULLET '|'
#define SKIN_INVADER 'M'
#define SKIN_INVADER_BULLET '*'
#define SKIN_WALL '='

#define INVADER_MOVE_TICKS 2

#define WALL_WIDTH 2
#define WALL_SPACE 2
#define WALL_COUNT 4

#define HEALTH_PLAYER 3
#define HEALTH_WALL 3
#define HEALTH_INVADER 1

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
	int health;
	int alive;
} Entity;

// Static Constructors
Vector2 new_s_Vector2(int x, int y);
Entity new_s_Entity(Vector2 position, char skin, int health);

// Dynamic Constructors
Entity* new_d_Entity(Vector2 position, char skin, int health);

// Memory Helpers
void FreeEntity(Entity** e_ptr){
	free(*e_ptr);
	*e_ptr = NULL;
}

// Update 
void UpdateEntity(Entity* e);

// Display
char** InitDisplay();
void ClearDisplay(char** display, int clearTerminal);
void Display(char** display);
void DrawEntity(char** display, Entity* e);
void DrawEntities(char** display, Entity* es, int count);

// Input
char GetCommand();

// Game
int Collision(Entity* a, Entity* b);
int MoveEntity(Entity* e, enum Direction dir);      // Returns 1 if entity moved, 0 if it didn't 
int MoveEntities(Entity** es, enum Direction dir);  // Returns 1 if all entities moved, 0 if they didn't
int Shoot(Entity* player, Entity** bullet_ptr);     // Returns 1 if shot was fired, 0 if it wasn't
Entity* InitPlayer();
Entity* InitWalls();
Entity** InitInvaders();
Vector2 PickRandomEntity(Entity** es, int x_size, int y_size);

int main(){
	char** display = InitDisplay();
	
	// Constant Game Components
	Entity** invaders = InitInvaders();
	Entity* walls = InitWalls();
	Entity* player = InitPlayer();
	
	// Variable Game Components
	Entity* player_bullet = NULL;
	Entity* rInvader = NULL;
	Entity* lInvader = NULL;

	// Game Variables
	enum Direction invaderMoveDirection = RIGHT;
	int ticks = -1; // Initialize ticks to -1 so that game begins at 0 ticks rather than 1
	
	// Game Loop
	int quit = 0;
	while (!quit){
		ClearDisplay(display, 0);

		/* Add entity drawing code here! */
		
		// Draw invader matrix
		for (int y = 0; y < Y_INVADER_COUNT; ++y){
			DrawEntities(display, invaders[y], X_INVADER_COUNT);
		}
		
		DrawEntities(display, walls, WALL_COUNT * WALL_WIDTH);
		DrawEntity(display, player);
		DrawEntity(display, player_bullet);

		Display(display);

		// Obtain input, match to keycode, if no match, advance game!
		char command = GetCommand();
		switch (command){
			// case K_MOVE_UP:
			//     MoveEntity(player, UP);
			//     break;
			// case K_MOVE_DOWN:
			//     MoveEntity(player, DOWN);
			//     break;
			case K_MOVE_LEFT:
				MoveEntity(player, LEFT);
				break;
			case K_MOVE_RIGHT:
				MoveEntity(player, RIGHT);
				break;
			case K_SHOOT:
				Shoot(player, &player_bullet);
				break;
			default:
				break;
		}
		
		/* Add game update code here! */
		
		// Get new rInvader & lInvader if the current ones aren't alive
		if (rInvader == NULL || !rInvader->alive){
			for (int y = 0; y < Y_INVADER_COUNT; ++y){
				for (int x = 0; x < X_INVADER_COUNT; ++x){
					// Get rInvader
					if (rInvader == NULL || invaders[y][x].position.x > rInvader->position.x){
						rInvader = &invaders[y][x];
					}
					
					// Get lInvader
					if (lInvader == NULL || invaders[y][x].position.x < lInvader->position.x){
						lInvader = &invaders[y][x];
					}
				}
			}	
		}	

		// Show R&L invader info (disable terminal clearing!)
		//printf("UPPER RIGHTMOST INVADER: (%d, %d)\n", rInvader->position.x, rInvader->position.y);
		//printf("UPPER LEFTMOST INVADER: (%d, %d)\n", lInvader->position.x, lInvader->position.y);

		// Move invaders when time is OK (Time is OK when it is a multiple of INVADER_MOVE_TICKS		
		if (ticks % INVADER_MOVE_TICKS == 0){		
			if (rInvader->position.x >= X_SIZE - 1){
				invaderMoveDirection = LEFT;
			}
			
			else if (lInvader->position.x <= 0){
				invaderMoveDirection = RIGHT;
			}

			// Move invaders towards appropriate direction
			MoveEntities(invaders, invaderMoveDirection);
		}

		// Try and move bullet, if it is unable to, meaning it has left bounds, destroy it!
		if (!MoveEntity(player_bullet, UP)){
			FreeEntity(&player_bullet);
		}

		// [Invader Update Loop]
		for (int y = 0; y < Y_INVADER_COUNT; ++y){
			for (int x = 0; x < X_INVADER_COUNT; ++x){	
				// Check if player bullet is colliding with something once it has moved, if so, destroy it
				if (Collision(player_bullet, &invaders[y][x])){
					// If so, deplete health of invader at the bullet's position and kill the bullet itself
					invaders[y][x].health--;
					FreeEntity(&player_bullet);
				}
			
				// Update invader at (x, y)
				UpdateEntity(&invaders[y][x]);
			}
		}

		// [Walls Update Loop]
		for (int i = 0; i < WALL_COUNT * WALL_WIDTH; ++i){
			// Make walls vulnerable to player bullets
			//if (Collision(player_bullet, &walls[i])){
			//	walls[i].health--;
			//	FreeEntity(&player_bullet);
			//}
			
			// Update wall
			UpdateEntity(&walls[i]);
		}

		// [Player Update]
		UpdateEntity(player);
		
		// DEBUG: Pick random invader [REMOVE THIS]
		Vector2 invaderPosition = PickRandomEntity(invaders, X_INVADER_COUNT, Y_INVADER_COUNT);
		printf("%d %d", invaderPosition.x, invaderPosition.y);

		// Advance ticks
		ticks++;
	}

	Display(display);

	return 0;
}

Vector2 new_s_Vector2(int x, int y){
	Vector2 n;

	n.x = x;
	n.y = y;

	return n;
}

Entity new_s_Entity(Vector2 position, char skin, int health){
	Entity e;

	e.position.x = position.x;
	e.position.y = position.y;

	e.skin = skin;

	e.health = health;

	// All entities are alive by default
	e.alive = 1;

	return e;
}

Entity* new_d_Entity(Vector2 position, char skin, int health){
	Entity* e = (Entity*)malloc(sizeof(Entity));

	e->position.x = position.x;
	e->position.y = position.y;

	e->skin = skin;
	
	e->health = health;

	// All entities are alive by default
	e->alive = 1;

	return e;
}

void UpdateEntity(Entity* e){
	// Kill entity on health deplete
	if (e->health <= 0){
		e->alive = 0;
	}
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

void ClearDisplay(char** display, int clearTerminal){
	// Clear console/terminal
	// NOTE This is not portable and works only in UNIX-like environments!
	if (clearTerminal)
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
			// Add color to display members
			switch (display[y][x]){
				case SKIN_PLAYER:
					printf(B_GREEN " %c " C_RESET, SKIN_PLAYER);	
					break;
				case SKIN_PLAYER_BULLET:
					printf(B_GREEN " %c " C_RESET, SKIN_PLAYER_BULLET); 
					break;	
				case SKIN_WALL:
					printf(B_GREEN " %c " C_RESET, SKIN_WALL);
					break;
				case SKIN_INVADER:
					printf(B_WHITE " %c " C_RESET, SKIN_INVADER);
					break;
				default:
					printf(" %c ", display[y][x]);
			}
		}
		puts("]");
	}
}

void DrawEntity(char** display, Entity* e){
	if (e == NULL){
		return;
	}
	
	// Add an entity to the display
	display[e->position.y][e->position.x] = e->skin;
}

void DrawEntities(char** display, Entity* es, int count){
	// Draw entities that are alive
	for (int i = 0; i < count; ++i){
		if (es[i].alive){
			DrawEntity(display, &es[i]);
		}
	}
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

int Collision(Entity* a, Entity* b){
	if (a == NULL || b == NULL || !a->alive || !b->alive)
		return 0;
	
	// Two entities are colliding if their positions are equal
	return (a->position.x == b->position.x) && (a->position.y == b->position.y);
}

int MoveEntity(Entity* e, enum Direction dir){
	// Do nothing if entity is not alive or is null
	if (e == NULL || !e->alive)
		return 0;

	// Determine position change
	int dx = 0;
	int dy = 0;
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

	// Apply position change to entity if it would end up in bounds
	if (e->position.x + dx >= 0 && e->position.x + dx < X_SIZE && e->position.y + dy >= 0 && e->position.y + dy < Y_SIZE){
		e->position.x += dx;
		e->position.y += dy;
		return 1;
	}

	return 0;
}

int MoveEntities(Entity** es, enum Direction dir){
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

	// Move entities that are alive, no in-bounds checking because we should do this outside this function
	for (int y = 0; y < Y_INVADER_COUNT; ++y){
		for (int x = 0; x < X_INVADER_COUNT; ++x){
			if (es[y][x].alive){
				es[y][x].position.x += dx;
				es[y][x].position.y += dy;
			}
		}
	}

	return 1;
}

int Shoot(Entity* player, Entity** bullet_ptr){
	// Do not create a new bullet if one already exists (only one bullet onscreen at a time)
	if (*bullet_ptr != NULL){
		return 0;
	}

	// If current bullet is free, make new one!
	*bullet_ptr = new_d_Entity(new_s_Vector2(player->position.x, player->position.y - 1), SKIN_PLAYER_BULLET, 1);
	return 1;
}

Entity* InitPlayer(){
	// Shorthand for creating the player
	Entity* player = new_d_Entity(new_s_Vector2(0, PLAYER_ROW), SKIN_PLAYER, HEALTH_PLAYER);
	return player;
}

Entity* InitWalls(){
	// Create a dynamic array containing the row of walls
	Entity* walls = (Entity*)malloc(WALL_COUNT * WALL_WIDTH * sizeof(Entity));

	// Initialize their x positions according to parameters
	int index = 0;
	for (int i = 0; i < WALL_COUNT; ++i){	
		for (int j = 0; j < WALL_WIDTH; ++j){
			walls[index] = new_s_Entity(new_s_Vector2(index + (i * WALL_SPACE), WALL_ROW), SKIN_WALL, HEALTH_WALL);
			index++;
		}
	}
	
	return walls;
}

Entity** InitInvaders(){
	// Allocate invader row
	Entity** rows = (Entity**)malloc(Y_INVADER_COUNT * sizeof(Entity*));

	// Allocate columns
	for (int y = 0; y < Y_INVADER_COUNT; ++y){
		rows[y] = (Entity*)malloc(X_INVADER_COUNT * sizeof(Entity));

		for (int x = 0; x < X_INVADER_COUNT; ++x){
			// Make invaders!
			rows[y][x] = new_s_Entity(new_s_Vector2(x, y), SKIN_INVADER, HEALTH_INVADER);
		}
	}

	return rows;
}

Vector2 PickRandomEntity(Entity** es, int x_size, int y_size){
	Vector2 possiblePositions[x_size * y_size];
	
	// Look at all living entities and copy their positions to possiblePositions
	int k = 0;
	for (int i = 0; i < y_size; ++i){
		for (int j = 0; j < x_size; ++j){
			if (es[i][j].alive){
				possiblePositions[k].x = es[i][j].position.x;
				possiblePositions[k].y = es[i][j].position.y;
				++k;
			}
		}
	}
	
	// Return random position
	return possiblePositions[rand() % k]; 
}

