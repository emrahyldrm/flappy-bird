#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
using namespace std;

#define GET_NAME(playerName) getstr(playerName)
#define FILE_NAME "bestScore.bin"
#define NAME_SIZE 100
#define JUMP 4
#define CRACK_SIZE 4
#define WAIT_BIG 140000
#define WAIT_LIT 110000
#define DOUBLE 2

void writeInfo(int row, int col);
void drawPipe(int begin, int end, int pipeCol, int row);
void drawStarting(int row, int col);
void readBest(int * bestScore, char bestPlayerName[]);
void writeBest(int bestScore, const char bestPlayerName[]);
void getPlayerName(char playerName);
void getNewPipeValue(int * crackStart, int * crackFinish, int row);
int controlCollision(int pipeCol, int birdCol, int birdRow, int crackStart,
					 int crackFinish);

int main() {
	srand(time(NULL));
	int score = 0, isScore = 0, bestScore = 0;
	int isOver = 0, flag = -1;
	;
	int crackStart1, crackFinish1, crackStart2, crackFinish2;
	int row, col, birdRow, birdCol, pipeCol1, pipeCol2;
	int pipeCounter, collision;
	int wait = WAIT_LIT;
	int command;
	char playerName[NAME_SIZE], bestPlayerName[NAME_SIZE];
	char bird = '@', ch;

	readBest(&bestScore, bestPlayerName);

	initscr();
	curs_set(0);
	keypad(stdscr, true);
	start_color();
	int mode = rand() % 2;
	// RANDOM FOREGROUND AND BACKGROUND COLORS
	if (mode == 0) {
		assume_default_colors(COLOR_BLACK, COLOR_CYAN);  // DAY MODE
	} else {
		assume_default_colors(COLOR_WHITE, COLOR_BLACK);  // NIGHT MODE
	}
	init_pair(1, COLOR_BLACK, COLOR_GREEN);  // PIPE COLORS
	int birdType = rand() % 3;
	// RANDOM BIRD COLORS
	if (birdType == 0) {
		init_pair(2, COLOR_BLACK, COLOR_YELLOW);
		if (mode == 0){
			init_pair(7, COLOR_YELLOW, COLOR_CYAN);
		} else {
			init_pair(7, COLOR_YELLOW, COLOR_BLACK);
		}
	} else if (birdType == 1) {
		init_pair(2, COLOR_BLACK, COLOR_RED);
		if (mode == 0){
			init_pair(7, COLOR_RED, COLOR_CYAN);
		} else {
			init_pair(7, COLOR_RED, COLOR_BLACK);
		}
	} else if (birdType == 2) {
		init_pair(2, COLOR_BLACK, COLOR_BLUE);
		if (mode == 0){
			init_pair(7, COLOR_BLUE, COLOR_CYAN);
		} else {
			init_pair(7, COLOR_BLUE, COLOR_BLACK);
		}
	}
	init_pair(3, COLOR_YELLOW, COLOR_YELLOW);  // UNDERGROUND COLORS
	init_pair(4, COLOR_BLACK, COLOR_WHITE);	// LOGO COLORS
	init_pair(5, COLOR_WHITE, COLOR_RED);	  // GAME OVER COLORS
	init_pair(6, COLOR_WHITE,
			  COLOR_GREEN);  // GET READY AND GROUND BORDER COLORS

	getmaxyx(stdscr, row, col);

	if (row < 24 || col < 75) {
		wait = WAIT_BIG;
	}

	writeInfo(row, col);
	GET_NAME(playerName);

	drawStarting(row, col);

	refresh();
	noecho();
	timeout(true);

	birdRow = row / 2;
	birdCol = col / 4;
	pipeCol1 = col;
	pipeCol2 = col;
	pipeCounter = col;

	getNewPipeValue(&crackStart1, &crackFinish1, row);
	getNewPipeValue(&crackStart2, &crackFinish2, row);

	while (!isOver) {
		clear();

		mvaddch(birdRow, birdCol, bird | COLOR_PAIR(2));

		if (score % 8 == 0) {
			mvprintw(1, col / 2 - 20, "SCORE : %d", score / 8);
		}

		mvprintw(0, col / 2 - 20, "%s", playerName);
		mvprintw(0, col / 2 + 13, "%s", bestPlayerName);
		mvprintw(1, col / 2 + 13, "BEST : %d", bestScore);

		attron(COLOR_PAIR(1));
		drawPipe(crackStart1, crackFinish1, pipeCol1, row);

		if (pipeCounter < col / 2) {
			drawPipe(crackStart2, crackFinish2, pipeCol2, row);
			pipeCol2--;
		}
		pipeCol1--;
		attroff(COLOR_PAIR(1));
		for (int i = 0; i < col; i++) {
			attron(COLOR_PAIR(3));
			mvprintw(row - 1, i, "#");
			mvprintw(row - 2, i, "#");
			attroff(COLOR_PAIR(3));
			attron(COLOR_PAIR(6));
			mvprintw(row - 3, i, "/");
			attroff(COLOR_PAIR(6));
			// mvprintw(2, i, "#");
		}

		if (birdRow < row - 1) {
			birdRow++;
		}

		command = getch();
		if (command == ' ') {
			flag++;
			birdRow -= JUMP;
		} else if (command == KEY_F(5)) {
			wait -= 10000;
		}

		if (birdRow < 2) {
			birdRow = 2;
		}

		collision = controlCollision(pipeCol1, birdCol, birdRow, crackStart1,
									 crackFinish1);
		if (collision) {
			if (collision == DOUBLE) {
				isOver = true;
			} else if (isScore) {
				score++;
				if (score / 8 > bestScore) {
					bestScore = score / 8;
					strcpy(bestPlayerName, playerName);
				}
			}
		}

		collision = controlCollision(pipeCol2, birdCol, birdRow, crackStart2,
									 crackFinish2);
		if (collision) {
			if (collision == DOUBLE) {
				isOver = true;
			} else if (isScore) {
				score++;
				if (score / 8 > bestScore) {
					bestScore = score / 8;
					strcpy(bestPlayerName, playerName);
				}
			}
		}

		if (birdRow > row - 4 /*|| birdRow == 2*/) {
			// TOUCH THE GROUND
			isOver = true;
		}

		if (pipeCol1 == 0) {
			getNewPipeValue(&crackStart1, &crackFinish1, row);
			pipeCol1 = col;
		}

		if (pipeCol2 == 0) {
			getNewPipeValue(&crackStart2, &crackFinish2, row);
			pipeCol2 = col;
			pipeCounter = col / 2;
		}

		isScore++;
		pipeCounter--;

		refresh();

		usleep(wait);
	}

	writeBest(bestScore, bestPlayerName);

	clear();
	/*
	  ____                         ___
	 / ___| __ _ _ __ ___   ___   / _ \__   _____ _ __
	| |  _ / _` | '_ ` _ \ / _ \ | | | \ \ / / _ \ '__|
	| |_| | (_| | | | | | |  __/ | |_| |\ V /  __/ |
	 \____|\__,_|_| |_| |_|\___|  \___/  \_/ \___|_|
	*/
	attron(A_BOLD | COLOR_PAIR(5));
	mvprintw(row / 2 - 10, (col - 51) / 2,
			 "  ____                         ___                 ");
	mvprintw(row / 2 - 9, (col - 51) / 2,
			 " / ___| __ _ _ __ ___   ___   / _ \\__   _____ _ __ ");
	mvprintw(row / 2 - 8, (col - 51) / 2,
			 "| |  _ / _` | '_ ` _ \\ / _ \\ | | | \\ \\ / / _ \\ '__|");
	mvprintw(row / 2 - 7, (col - 51) / 2,
			 "| |_| | (_| | | | | | |  __/ | |_| |\\ V /  __/ |   ");
	mvprintw(row / 2 - 6, (col - 51) / 2,
			 " \\____|\\__,_|_| |_| |_|\\___|  \\___/  \\_/ \\___|_|   ");
	attroff(A_BOLD | COLOR_PAIR(5));
	mvprintw(row / 2 - 1, col / 2 - 20, "SCORE : %d", score / 8);
	mvprintw(row / 2 - 2, col / 2 - 20, "%s", playerName);
	mvprintw(row / 2 - 2, col / 2 + 10, "%s", bestPlayerName);
	mvprintw(row / 2 - 1, col / 2 + 10, "BEST : %d", bestScore);
	mvprintw(row / 2 + 1, (col - 39) / 2, "Best score was saved to \"%s\"",
			 FILE_NAME);
	refresh();

	getchar();

	endwin();
	return 0;
}

void writeInfo(int row, int col) {
	/*
	 _____ _                         ____  _         _
	|  ___| | __ _ _ __  _ __  _   _| __ )(_)_ __ __| |
	| |_  | |/ _` | '_ \| '_ \| | | |  _ \| | '__/ _` |
	|  _| | | (_| | |_) | |_) | |_| | |_) | | | | (_| |
	|_|   |_|\__,_| .__/| .__/ \__, |____/|_|_|  \__,_|
	              |_|   |_|    |___/
	*/
	attron(A_BOLD | COLOR_PAIR(4));
	mvprintw(row / 2 - 14, (col - 50) / 2,
			 " _____ _                         ____  _         _ ");
	mvprintw(row / 2 - 13, (col - 50) / 2,
			 "|  ___| | __ _ _ __  _ __  _   _| __ )(_)_ __ __| |");
	mvprintw(row / 2 - 12, (col - 50) / 2,
			 "| |_  | |/ _` | '_ \\| '_ \\| | | |  _ \\| | '__/ _` |");
	mvprintw(row / 2 - 11, (col - 50) / 2,
			 "|  _| | | (_| | |_) | |_) | |_| | |_) | | | | (_| |");
	mvprintw(row / 2 - 10, (col - 50) / 2,
			 "|_|   |_|\\__,_| .__/| .__/ \\__, |____/|_|_|  \\__,_|");
	mvprintw(row / 2 - 9, (col - 50) / 2,
			 "              |_|   |_|    |___/                   ");
	attroff(A_BOLD | COLOR_PAIR(4));
	/*
	            [][][][][][]          
            [][]      []    []        
          []        []        []      
	  [][][][]      []      []  []    
	[]        []    []      []  []    
	[]          []    []        []    
	[]          []      [][][][][][]  
	  []      []      []            []
	    [][][]      []  [][][][][][]  
	    []            []          []  
	      [][]          [][][][][]    
	          [][][][][]              
	*/
	attron(COLOR_PAIR(7));
	mvprintw(row / 2 - 7, (col - 34) / 2,
			 "            [][][][][][]          ");
	mvprintw(row / 2 - 6, (col - 34) / 2,
			 "        [][]      []    []        ");
	mvprintw(row / 2 - 5, (col - 34) / 2,
			 "      []        []        []      ");
	mvprintw(row / 2 - 4, (col - 34) / 2,
			 "  [][][][]      []      []  []    ");
	mvprintw(row / 2 - 3, (col - 34) / 2,
			 "[]        []    []      []  []    ");
	mvprintw(row / 2 - 2, (col - 34) / 2,
			 "[]          []    []        []    ");
	mvprintw(row / 2 - 1, (col - 34) / 2,
			 "[]          []      [][][][][][]  ");
	mvprintw(row / 2, (col - 34) / 2,
			 "  []      []      []            []");
	mvprintw(row / 2 + 1, (col - 34) / 2,
			 "    [][][]      []  [][][][][][]  ");
	mvprintw(row / 2 + 2, (col - 34) / 2,
			 "    []            []          []  ");
	mvprintw(row / 2 + 3, (col - 34) / 2,
			 "      [][]          [][][][][]    ");
	mvprintw(row / 2 + 4, (col - 34) / 2,
			 "          [][][][][]              ");
	attroff(COLOR_PAIR(7));
	mvprintw(row / 2 + 6, (col - 62) / 2,
			 "If you touch the ground or any of the pipes, game will be over");
	mvprintw(row / 2 + 8, (col - 27) / 2, "Use \"SPACE BAR\" for playing");
	mvprintw(row / 2 + 12, (col - 27) / 2, "Before, Enter Your Name >> ");
}

int controlCollision(int pipeCol, int birdCol, int birdRow, int crackStart,
					 int crackFinish) {
	int status = false;

	if (pipeCol - 8 == birdCol || pipeCol - 7 == birdCol ||
		pipeCol - 6 == birdCol || pipeCol - 5 == birdCol ||
		pipeCol - 4 == birdCol || pipeCol - 3 == birdCol ||
		pipeCol - 2 == birdCol || pipeCol - 1 == birdCol) {
		status++;
		if (birdRow < crackStart || birdRow > crackFinish) {
			status++;
		}
	}

	return status;
}

void drawPipe(int begin, int end, int pipeCol, int row) {
	for (int i = 0; i < row - 3; i++) {
		if (i < begin) {
			if (i == begin - 1 || i == begin - 2) {
				mvprintw(i, pipeCol - 9, "          ");
			} else if (i == begin - 3) {
				mvprintw(i, pipeCol - 8, "________");
			} else{
				//                       9876543210
				mvprintw(i, pipeCol - 8, "        ");
			}
		}
		if (i > end) {
			if (i == end + 1) {
				mvprintw(i, pipeCol - 9, "          ");
			} else if (i == end + 2) {
				mvprintw(i, pipeCol - 9, " ________ ");
			} else {
				mvprintw(i, pipeCol - 8, "        ");
			}
		}
	}
}

void drawStarting(int row, int col) {
	clear();
	/*
	  ____      _     ____                _       _
	 / ___| ___| |_  |  _ \ ___  __ _  __| |_   _| |
	| |  _ / _ \ __| | |_) / _ \/ _` |/ _` | | | | |
	| |_| |  __/ |_  |  _ <  __/ (_| | (_| | |_| |_|
	 \____|\___|\__| |_| \_\___|\__,_|\__,_|\__, (_)
	                                        |___/
	*/
	attron(A_BOLD | COLOR_PAIR(6));
	mvprintw(row / 2 - 10, (col - 48) / 2,
			 "  ____      _     ____                _       _ ");
	mvprintw(row / 2 - 9, (col - 48) / 2,
			 " / ___| ___| |_  |  _ \\ ___  __ _  __| |_   _| |");
	mvprintw(row / 2 - 8, (col - 48) / 2,
			 "| |  _ / _ \\ __| | |_) / _ \\/ _` |/ _` | | | | |");
	mvprintw(row / 2 - 7, (col - 48) / 2,
			 "| |_| |  __/ |_  |  _ <  __/ (_| | (_| | |_| |_|");
	mvprintw(row / 2 - 6, (col - 48) / 2,
			 " \\____|\\___|\\__| |_| \\_\\___|\\__,_|\\__,_|\\__, (_)");
	mvprintw(row / 2 - 5, (col - 48) / 2,
			 "                                        |___/   ");
	attroff(A_BOLD | COLOR_PAIR(6));
	mvprintw(row / 2 + 1, (col - 5) / 2 + 3, "]");
	mvprintw(row / 2 + 1, (col - 5) / 2 - 1, "[");

	for (int j = 1; j < 4; j++) {
		for (int i = 0; i < j; i++) {
			mvprintw(row / 2 + 1, (col - 5) / 2 - 1 + j, "#");
			refresh();
		}
		sleep(1);
	}
}

void readBest(int * bestScore, char bestPlayerName[]) {
	FILE * bestFilePtr;

	bestFilePtr = fopen(FILE_NAME, "rb");
	if (bestFilePtr != NULL) {
		fread(bestScore, sizeof(int), 1, bestFilePtr);
		fread(bestPlayerName, sizeof(char) * NAME_SIZE, 1, bestFilePtr);
		fclose(bestFilePtr);
	}
}

void writeBest(int bestScore, const char bestPlayerName[]) {
	FILE * bestFilePtr;

	bestFilePtr = fopen(FILE_NAME, "wb");
	fwrite(&bestScore, sizeof(int), 1, bestFilePtr);
	fwrite(bestPlayerName, sizeof(char) * NAME_SIZE, 1, bestFilePtr);

	fclose(bestFilePtr);
}

void getNewPipeValue(int * crackStart, int * crackFinish, int row) {
	*crackStart = rand() % row / 2 + 3;
	*crackFinish = *crackStart + CRACK_SIZE;
}
