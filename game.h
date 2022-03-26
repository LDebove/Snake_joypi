#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

// nombre de pommes sur le plateau de jeu
#define APPLE_NUMBER 5

// valeurs possibles de directions
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

// liste chaînée représentant les parties du serpent
typedef struct {
    int x;
    int y;
    struct SnakePart *next_part; // prochaine partie du serpent
} SnakePart;

// structure correspondant à la pomme
typedef struct {
    int x;
    int y;
} Apple;

// structure représentant le plateau de jeu
typedef struct {
    SnakePart *snake; // première partie du serpent
    Apple apples[APPLE_NUMBER]; // tableau de pommes
    int col; // nombre de colonnes du plateau de jeu (nombre de caractère maximum sur la largeur du terminal)
    int row; // nombre de lignes du plateau de jeu (nombre de caractère maximum sur la hauteur du terminal)
} GameBoard;

GameBoard *create_game_board(SnakePart *snake, int col, int row);
SnakePart *create_snake(int x, int y);
SnakePart *create_snake_part(int x, int y);
void place_apple(GameBoard *game_board, int apple_id);
void display_snake(SnakePart *snake);
void display_apples(Apple *apples);
bool is_occupied(GameBoard *game_board, int x, int y);
bool is_occupied_by_snake(GameBoard *game_board, int x, int y);
bool is_occupied_by_apple(GameBoard *game_board, int x, int y);
bool move_snake(GameBoard *game_board, int direction, int *score);
SnakePart *check_move(GameBoard *game_board, int direction);
void remove_snake_tail(GameBoard *game_board);
void find_and_replace_apple(GameBoard *game_board, int x, int y);

void translate_led_map(GameBoard *led_game_board, unsigned char led_grid[8]);
GameBoard *scale_board(GameBoard *game_board);
unsigned char translate_register_from_x(int x);
