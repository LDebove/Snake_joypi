#include "game.h"

// retourne un nouveau plateau de jeu
GameBoard *create_game_board(SnakePart *snake, int col, int row)
{
    GameBoard *game_board = malloc(sizeof(GameBoard));
    game_board->snake = snake;
    game_board->col = col;
    game_board->row = row;
    return game_board;
}

// retourne un nouveau serpent
SnakePart *create_snake(int x, int y)
{
    SnakePart *snake = create_snake_part(x, x);
    return snake;
}

// retourne une partie de serpent
SnakePart *create_snake_part(int x, int y)
{
    SnakePart *snake_part = malloc(sizeof(SnakePart));
    snake_part->x = x;
    snake_part->y = y;
    snake_part->next_part = NULL;
    return snake_part;
}

// place la pomme définie par son identifiant à des coordonnées aléatoires dans le plateau de jeu
void place_apple(GameBoard *game_board, int apple_id)
{
    int apple_x, apple_y;
    do {
        apple_x = rand()%game_board->col;
        apple_y = rand()%game_board->row;
    } while(is_occupied(game_board, apple_x, apple_y));
    game_board->apples[apple_id].x = apple_x;
    game_board->apples[apple_id].y = apple_y;
}

// affiche le caractère associé au serpent aux coordonnées des parties du serpent 
void display_snake(SnakePart *snake)
{
    while(snake) {
        mvaddch(snake->y, snake->x, (chtype) 'X');
        snake = snake->next_part;
    }
}

// affiche le caractère associé à la pomme aux coordonnées de la pomme 
void display_apples(Apple *apples)
{
    for(int i=0; i<APPLE_NUMBER; i++) {
        mvaddch(apples[i].y, apples[i].x, (chtype) 'O');
    }
}

// retourne true si les coordonnées définies par x et y sont occupées par un élément du plateau de jeu, retourne false sinon 
bool is_occupied(GameBoard *game_board, int x, int y)
{
    if(is_occupied_by_snake(game_board, x, y)) return true; // vérifie si les coordonnées correspondent à une partie du serpent
    if(is_occupied_by_apple(game_board, x, y)) return true; // vérifie si les coordonnées correspondent à une pomme
    return false;
}

// retourne true si les coordonnées définies par x et y sont occupées par une partie du serpent, retourne false sinon
bool is_occupied_by_snake(GameBoard *game_board, int x, int y)
{
    SnakePart *snake = game_board->snake;
    while(snake) {
        if(snake->x == x && snake->y == y) return true;
        snake = snake->next_part;
    }
    return false;
}

// retourne true si les coordonnées définies par x et y sont occupées par une pomme, retourne false sinon
bool is_occupied_by_apple(GameBoard *game_board, int x, int y)
{
    for(int i; i<APPLE_NUMBER; i++) {
        if(game_board->apples[i].x == x && game_board->apples[i].y == y) return true;
    }
    return false;
}

// retourne false si le serpent rencontre le bord du plateau ou une partie du serpent, retourne true sinon
// crée une nouvelle tête pour le serpent et greffe le serpent à cette tête
// en cas de rencontre avec une pomme, incrémente de 5 le score, sinon détruit la dernière partie du serpent
bool move_snake(GameBoard *game_board, int direction, int *score)
{
    // vérifie la collision avec un mur et crée la nouvelle tête du serpent
    SnakePart *new_snake_head = check_move(game_board, direction);
    if(new_snake_head == NULL) return false;

    // vérifie la collision avec une partie du serpent
    if(is_occupied_by_snake(game_board, new_snake_head->x, new_snake_head->y)) return false;

    new_snake_head->next_part = game_board->snake;
    game_board->snake = new_snake_head;

    // vérifie la collision avec une pomme
    // en cas de rencontre avec une pomme, incrémente de 5 le score, sinon détruit la dernière partie du serpent
    if(!is_occupied_by_apple(game_board, new_snake_head->x, new_snake_head->y)) {
        remove_snake_tail(game_board); // supprime la dernière partie du serpent
    } else {
        *score += 5; // incrémente le score de 5
        find_and_replace_apple(game_board, new_snake_head->x, new_snake_head->y); // change la position de la pomme que le serpent a rencontré
    }

    return true;
}

// retourne une partie de serpent avec les nouvelles coordonnées impliquées par la direction si les nouvelles coordonnées font partie des limites du plateau, retourne NULL sinon
SnakePart *check_move(GameBoard *game_board, int direction)
{
    SnakePart *snake = game_board->snake;
    int new_x = snake->x, new_y = snake->y;
    switch(direction) {
        case UP:
        new_y = snake->y - 1;
        break;
        
        case DOWN:
        new_y = snake->y + 1;
        break;
        
        case LEFT:
        new_x = snake->x - 1;
        break;

        case RIGHT:
        new_x = snake->x + 1;
        break;
    }
    if(new_x < 0 || new_y < 0 || new_x > game_board->col || new_y > game_board->row) return NULL;
    else return create_snake_part(new_x, new_y);
}

// supprime la dernière partie du serpent
void remove_snake_tail(GameBoard *game_board)
{
    SnakePart *current_part, *previous_part = NULL;
    current_part = game_board->snake;
    while(true) {
        if(current_part->next_part != NULL) {
            previous_part = current_part;
            current_part = current_part->next_part;
        } else {
            break;
        }
    }
    previous_part->next_part = NULL;
    free(current_part); // libère la mémoire prise par la structure
}

// trouve la pomme dont les coordonnées correspondent à x et y et changent ses coordonnées
void find_and_replace_apple(GameBoard *game_board, int x, int y)
{
    int i;
    for(i = 0; i < APPLE_NUMBER; i++) {
        if(game_board->apples[i].x == x && game_board->apples[i].y == y) break;
    }
    place_apple(game_board, i);
}

// traduit un plateau de jeu de 8x8 en un tableau de 8 x 8 bits (unsigned char)
void translate_led_map(GameBoard *led_game_board, unsigned char led_grid[8])
{
    unsigned char _led_grid[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // initialisation du tableau de 8 x 8 bits à 0
    int i;
    for(i = 0; i < 8; i++) led_grid[i] = _led_grid[i];

    // placement du serpent dans le tableau de 8 x 8 bits
    SnakePart *led_snake = led_game_board->snake;
    int x, y;
    while(led_snake != NULL) {
        x = led_snake->x;
        y = led_snake->y;

        if(x > 7) x = 7;
        if(x < 0) x = 0;
        if(y > 7) y = 7;
        if(y < 0) y = 0;
        led_grid[y] |= translate_register_from_x(x);

        led_snake = led_snake->next_part;
    }

    // placement des pommes dans le tableau de 8 x 8 bits
    for(i = 0; i < APPLE_NUMBER; i++) {
        x = led_game_board->apples[i].x;
        y = led_game_board->apples[i].y;

        if(x > 7) x = 7;
        if(x < 0) x = 0;
        if(y > 7) y = 7;
        if(y < 0) y = 0;
        led_grid[y] |= translate_register_from_x(x);
    }
}

// retourne un plateau de jeu de 8x8 mis à l'échelle par rapport à un plateau de jeu entré en paramètre
GameBoard *scale_board(GameBoard *game_board)
{
	GameBoard *led_game_board = create_game_board(create_snake(game_board->snake->x, game_board->snake->y), 8, 8); // création du plateau de jeu de 8x8

    double height_ratio = (double) (led_game_board->row) / game_board->row, width_ratio = (double) (led_game_board->col) / game_board->col;

    // mise à l'échelle du serpent
    SnakePart *snake = game_board->snake, *led_snake = led_game_board->snake;

    // mise à l'échelle des coordonnées du serpent sur un plateau de 8x8
    while(snake != NULL) {
        led_snake->x = (int) (snake->x * width_ratio);
        led_snake->y = (int) (snake->y * height_ratio);
        if(snake->next_part != NULL) led_snake->next_part = create_snake_part(0, 0);
        led_snake = led_snake->next_part;
        snake = snake->next_part;
    }

    // mise à l'échelle des pommes
    // mise à l'échelle des coordonnées des pommes sur un plateau de 8x8
    for(int i = 0; i < APPLE_NUMBER; i++) {
        led_game_board->apples[i].x = (int) (game_board->apples[i].x * width_ratio);
        led_game_board->apples[i].y = (int) (game_board->apples[i].y * height_ratio);
    }

    return led_game_board;
}

// retourne un octet correspondant à la position d'un élément du plateau de jeu après que celui ci ait été mis à l'échelle
unsigned char translate_register_from_x(int x)
{
    unsigned char matrix_register;

    switch(x) {
        case 0:
        matrix_register = 0b10000000;
        break;

        case 1:
        matrix_register = 0b1000000;
        break;

        case 2:
        matrix_register = 0b100000;
        break;

        case 3:
        matrix_register = 0b10000;
        break;

        case 4:
        matrix_register = 0b1000;
        break;

        case 5:
        matrix_register = 0b100;
        break;

        case 6:
        matrix_register = 0b10;
        break;

        case 7:
        matrix_register = 0b1;
        break;
    }

    return matrix_register;
}