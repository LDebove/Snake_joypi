#include "joypi.h"
#include "game.h"

int main(void)
{
    int seven_segment_fd;
    init_joypi(&seven_segment_fd); // initialisation des composants de la mallette JoyPi que l'on utilise
    setup_event_listeners(); // activation des interruption sur les front lors d'un appui sur les boutons de la croix directionnelle
    init_ncurses(); // initialise la librairie ncurse pour notre programme

    int col, row;
    getmaxyx(stdscr, row, col); // associe les valeurs maximales de hauteur et de largeur de notre terminale aux variables row et col

    GameBoard *game_board = create_game_board(create_snake(5, 5), col, row), *led_game_board; // création du plateau de jeu et placement du serpent sur le plateau de jeu

    for(int i=0; i<APPLE_NUMBER; i++) place_apple(game_board, i); // premier placement des pommes sur le plateau de jeu

    int score = 0;
    unsigned char led_map[8];

    g_direction = J_RIGHT; // direction par défaut du serpent sur le plateau de jeu

    while(true) {
        clear(); // efface l'écran
        display_snake(game_board->snake); // affichage du serpent sur le plateau de jeu
        display_apples(game_board->apples); // affichage des pommes sur le plateau de jeu
        refresh(); // affiche l'écrant
        g_direction = get_direction(g_direction); // détermination de la direction
        led_game_board = scale_board(game_board); // mise à l'échelle du plateau de jeu en un plateau de 8x8
        translate_led_map(led_game_board, led_map); // traduction du plateau de jeu de 8x8 en un tableau de 8x8 bits
        display_led_map(led_map); // affichage du tableau de 8x8 bits sur la matrice LED
        display_score(seven_segment_fd, score); // affichage du score sur les afficheurs 7-segments
        if(!move_snake(game_board, g_direction, &score)) break; // déplacement ou non du serpent sur le plateau de jeu
    }
    
    endwin(); // arrêt de l'interface graphique ncurses
    clear_LED(); // extinction des LED sur la matrice LED
    stop_MAX7219(); // arrêt de la matrice LED
    return 0;
}