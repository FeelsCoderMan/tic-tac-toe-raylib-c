tictactoe: main.c
	cc -Wall -Wextra -Wno-missing-braces -std=c99 main.c -o ticTacToe -I include -L lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

