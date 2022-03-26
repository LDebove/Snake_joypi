#include "joypi.h"

int g_direction; // variable globale de direction modifiée par l'appui sur les boutons de la croix directionnelle

// initialise les modules de la mallette JoyPi que l'on utilise
void init_joypi(int *seven_segment_fd)
{
    wiringPiSetup(); // setup la librairie wiringPi
    init_gpio_ports(); // configuration en I/O des ports GPIO 
	wiringPiSPISetup(0, 32000000); // initialisation du bus SPI pour la librairie wiringPi
    init_MAX7219(); // configuration du MAX7219 pour la matrice LED
	*seven_segment_fd = wiringPiI2CSetup(0x70); // initialisation du bus I2C pour la librairie wiringPi
	init_7segment(*seven_segment_fd); // configuration des afficheurs 7-segments
}

// initialise les ports gpio en entrée ou en sortie
void init_gpio_ports(void)
{
    // configuration en sortie des ports des boutons de la croix directionnelle
    pinMode(UP_BUTTON_PORT, OUTPUT); 
    pinMode(DOWN_BUTTON_PORT, OUTPUT);
    pinMode(LEFT_BUTTON_PORT, OUTPUT);
    pinMode(RIGHT_BUTTON_PORT, OUTPUT);

    // configuration en sortie des ports du MAX7219 pour la matrice LED
    pinMode(MOSI_PORT, OUTPUT);
	pinMode(CLK_PORT, OUTPUT);
	pinMode(LOAD_PORT, OUTPUT);
	digitalWrite(LOAD_PORT, 1);
}

// configure le MAX7219
void init_MAX7219()
{
	write_MAX7219(DECODE_MODE, 0x00);	// désactive le decode-mode
	write_MAX7219(INTENSITY, 0x01);		// met l'intensité des LED à 1
	write_MAX7219(SCAN_LIMIT, 0x07);	// non utilisé
	write_MAX7219(SHUTDOWN,0x01);		// active la matrice LED
	write_MAX7219(DISPLAY_TEST, 0x00);	// désactive la fonction d'affichage "test"
    clear_LED();
}

// configure les afficheurs 7-segments
void init_7segment(int seven_segment_fd)
{
	wiringPiI2CWrite(seven_segment_fd, 0x21);
	wiringPiI2CWrite(seven_segment_fd, HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | ((unsigned char) HT16K33_BLINK_OFF << 1));
	wiringPiI2CWrite(seven_segment_fd, HT16K33_CMD_BRIGHTNESS | (unsigned char) 10);
}

// désactive la matrice LED
void stop_MAX7219()
{
    write_MAX7219(SHUTDOWN, 0x00);
}

// active des interruptions sur les fronts pour l'appui sur les boutons de la croix directionnelle
void setup_event_listeners(void)
{
    wiringPiISR(UP_BUTTON_PORT, INT_EDGE_FALLING, up_button);	    //active un écouteur pour une interruption lors d'un front descendant sur le gpio du bouton haut
    wiringPiISR(DOWN_BUTTON_PORT, INT_EDGE_FALLING, down_button);	//active un écouteur pour une interruption lors d'un front descendant sur le gpio du bouton bas
	wiringPiISR(LEFT_BUTTON_PORT, INT_EDGE_FALLING, left_button);	//active un écouteur pour une interruption lors d'un front descendant sur le gpio du bouton gauche
	wiringPiISR(RIGHT_BUTTON_PORT, INT_EDGE_FALLING, right_button);	//active un écouteur pour une interruption lors d'un front descendant sur le gpio du bouton droit
}

// écris un octet dans le MAX7219
void write_byte_MAX7219(unsigned char data)
{
	for(int i = 0; i < 8; i++) {
		digitalWrite(CLK_PORT, 0);
		digitalWrite(MOSI_PORT, data & 0x80);
		delayMicroseconds(1);
		data <<= 1;
		digitalWrite(CLK_PORT, 1);
	}
}

// écris l'octet de données à l'addresse définie par l'octet d'addresse
void write_MAX7219(unsigned char register_adress, unsigned char data_to_write)
{
	digitalWrite(LOAD_PORT, 0);
	write_byte_MAX7219(register_adress);
	write_byte_MAX7219(data_to_write);
	digitalWrite(LOAD_PORT, 1);
}

// éteint toutes les LED de la matrice LED
void clear_LED()
{
	write_MAX7219(0x01, 0x00);
	write_MAX7219(0x02, 0x00);
	write_MAX7219(0x03, 0x00);
	write_MAX7219(0x04, 0x00);
	write_MAX7219(0x05, 0x00);
	write_MAX7219(0x06, 0x00);
	write_MAX7219(0x07, 0x00);
	write_MAX7219(0x08, 0x00);
}

// fonction liée à l'appui sur le bouton haut de la croix directionnelle
// change la variable globale de direction en fonction
void up_button(void)
{
	g_direction = J_UP;
}

// fonction liée à l'appui sur le bouton bas de la croix directionnelle
// change la variable globale de direction en fonction
void down_button(void)
{
    g_direction = J_DOWN;
}

// fonction liée à l'appui sur le bouton gauche de la croix directionnelle
// change la variable globale de direction en fonction
void left_button(void)
{
    g_direction = J_LEFT;
}

// fonction liée à l'appui sur le bouton droite de la croix directionnelle
// change la variable globale de direction en fonction
void right_button(void)
{
    g_direction = J_RIGHT;
}

// initialise la librairie ncurses sur la console
void init_ncurses(void)
{
    initscr();
    cbreak();		// pas de buffer de ligne
    noecho();		// pas d'echo des input getchar
    curs_set(0);	// désactive le curseur
}

// écris le tableau de 8x8 bits sur la matrice LED
void display_led_map(unsigned char led_map[8])
{
	for(int i = 0; i < 8; i++) {
		write_MAX7219((unsigned char) (i + 1), led_map[i]);
	}
}

// décompose et affiche le score sur les afficheurs 7-segments
void display_score(int seven_segment_fd, int score)
{
	int ones, tens, hundreds, thousands;
	int number[4];
	unsigned char adress = 0x00;

	thousands = score * 0.001;
	number[0] = thousands;
    score -= thousands * 1000;
    hundreds = score * 0.01;
	number[1] = hundreds;
    score -= hundreds * 100;
    tens = score * 0.1;
	number[2] = tens;
    score -= tens * 10;
    ones = score;
	number[3] = ones;

	unsigned char digit_number_data[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

	for(int i = 0; i < 4; i++) { 
		if(i == 2) adress += 2;
		wiringPiI2CWriteReg8(seven_segment_fd, adress++, digit_number_data[number[i]] & 0xFF); 
		wiringPiI2CWriteReg8(seven_segment_fd, adress++, digit_number_data[number[i]] >> 8);  
	}
}

// retourne la direction de la variable globale g_direction si il ne s'agit pas de la direction inverse de la direction précédente, retourne la direction précédente sinon
int get_direction(int previous_direction)
{
	delay(100);
    switch(g_direction) {
        case J_UP:
        if(previous_direction != J_DOWN) return J_UP;
        return previous_direction;
        break;

        case J_DOWN:
        if(previous_direction != J_UP) return J_DOWN;
        return previous_direction;
        break;

        case J_LEFT:
        if(previous_direction != J_RIGHT) return J_LEFT;
        return previous_direction;
        break;

        case J_RIGHT:
        if(previous_direction != J_LEFT) return J_RIGHT;
        return previous_direction;
        break;

        default:
        return previous_direction;
        break;
    }
}