all: joypi.o game.o main.c
	gcc joypi.o game.o main.c -lwiringPi -lncurses -o exe
joypi.o: joypi.c joypi.h
	gcc -c joypi.c -lwiringPi -lncurses
game.o: game.c game.h
	gcc -c game.c -lncurses

clean:
	rm -rf *.o exe