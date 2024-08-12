#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>

#define COLUMNS 50
#define ROWS 20
#define MAX_SNAKE_SIZE 256
#define DEFAULT_GAME_SLEEP_PERIOD 150
#define WALL_CHARACTER '#'
#define APPLE_CHARACTER 'O'
#define SNAKE_HEAD_CHARACTER '@'
#define SNAKE_BODY_CHARACTER '*'
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

struct SnakePart {
	int x;
	int y;
	char ch;
};

struct Snake {
	struct SnakePart parts[MAX_SNAKE_SIZE];
};

struct Apple {
	int x;
	int y;
	int exists; // acts like a boolean value
	char ch;
};

struct Game {
	int is_game_over;
	int sleep_period;
};

char board[COLUMNS * ROWS];
struct Snake snake;
struct Apple apple;
struct Game game;
char previousInput = 'D';

void generate_empty_board() {
	for (int y = 0; y < ROWS; y++) {
		for (int x = 0; x < COLUMNS; x++) {

			if (x == 0 || y == 0 || x == COLUMNS - 1 || y == ROWS - 1) {
				board[y * COLUMNS + x] = WALL_CHARACTER;
			}
			else {
				board[y * COLUMNS + x] = ' ';
			}
		}
	}
}

void generate_snake() {
	for (int i = 0; i < MAX_SNAKE_SIZE; i++) {
		char ch = snake.parts[i].ch;
		int x = snake.parts[i].x;
		int y = snake.parts[i].y;

		if (ch != '\0') {
			board[y * COLUMNS + x] = ch;
		}
	}
}

int get_random_number(int min, int max) {
	return rand() % ((max + 1) - min) + min;
}

void generate_apple() {
	
	if (apple.exists) {
		board[apple.y * COLUMNS + apple.x] = apple.ch; // render the apple
		return;
	}
	
	// Generate and render the apple
	apple.x = get_random_number(1, COLUMNS - 2);
	apple.y = get_random_number(1, ROWS - 2);

	board[apple.y * COLUMNS + apple.x] = apple.ch;
	apple.exists = 1;
}

void generate_board() {
	generate_empty_board();
	generate_apple();
	generate_snake();
}

void draw_board() {
	gotoxy(0, 0);
	for (int y = 0; y < ROWS; y++) {
		for (int x = 0; x < COLUMNS; x++) {
			putchar(board[y * COLUMNS + x]);
		}
		putchar('\n');
	}
}

void move_snake(int x, int y) {
	for (int i = MAX_SNAKE_SIZE-1; i > 0; i--) {
		snake.parts[i].x = snake.parts[i-1].x;
		snake.parts[i].y = snake.parts[i-1].y;
	}
	snake.parts[0].x += x;
	snake.parts[0].y += y;
}

void handle_user_input() {
	char character;
	if (_kbhit()) {
		character = (char)getchar();
	}
	else {
		character = previousInput;
	}

	if ((character == 'W' || character == 'w') && !(previousInput == 'S' || previousInput == 's')) {
		move_snake(0, -1);
		previousInput = character;
	}
	else if ((character == 'S' || character == 's') && !(previousInput == 'W' || previousInput == 'w')) {
		move_snake(0, 1);
		previousInput = character;
	}
	else if ((character == 'A' || character == 'a') && !(previousInput == 'D' || previousInput == 'd')) {
		move_snake(-1, 0);
		previousInput = character;
	}
	else if ((character == 'D' || character == 'd') && !(previousInput == 'A' || previousInput == 'a')) {
		move_snake(1, 0);
		previousInput = character;
	}
}

void initialize_snake() {
	snake.parts[0].x = (int)COLUMNS / 2;
	snake.parts[0].y = (int)ROWS / 2;
	snake.parts[0].ch = SNAKE_HEAD_CHARACTER;

	// Not sure if the code below is needed. 
	// I think that the default value is always '\0'

	/*for (int i = 1; i < MAX_SNAKE_SIZE; i++) {
		snake.parts[i].ch = '\0';
	}*/
}

void hide_cursor() {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
}

void configure_terminal() {
	hide_cursor();

	/* Disable echo mode and line input mode */
	HANDLE consoleHandle = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(consoleHandle, &mode);
	SetConsoleMode(consoleHandle, mode & (~ENABLE_ECHO_INPUT)); // Disable echo
	SetConsoleMode(consoleHandle, mode & (~ENABLE_LINE_INPUT)); // Disable line input
}

void initialiize_libraries() {
	srand(time(NULL));
}

void initialize_apple() {
	apple.ch = APPLE_CHARACTER;
	apple.exists = 0;
}

void game_over() {
	game.is_game_over = 1;
}

void grow_snake() {
	int index = 0;
	int pos = -1;
	while (pos < 0 && index < MAX_SNAKE_SIZE) {
		if (snake.parts[index].ch == '\0') {
			pos = index;
		}
		index++;
	}

	snake.parts[pos].ch = SNAKE_BODY_CHARACTER;
}

void handle_collision() {
	/* Handles all the collisions */

	// Note: the board has not been updated but the snake position has

	char collider = board[snake.parts[0].y * COLUMNS + snake.parts[0].x];

	switch (collider) {
		case WALL_CHARACTER:
			game_over();
			break;
		case SNAKE_BODY_CHARACTER:
			game_over();
			break;
		case APPLE_CHARACTER:
			grow_snake();
			apple.exists = 0;
			break;
		default:
			break;
	}
}

void clear_entire_terminal_screen() {
	system("cls");
}

void initialize_game() {
	game.is_game_over = 0;

	clear_entire_terminal_screen();
	initialiize_libraries();
	configure_terminal();
	initialize_apple();
	initialize_snake();
}

int main(int argc, char* argv[]) {
	initialize_game();

	while (!game.is_game_over) {
		generate_board();
		draw_board();
		handle_user_input();
		handle_collision();
		Sleep(DEFAULT_GAME_SLEEP_PERIOD);
	}
	return 0;
}
