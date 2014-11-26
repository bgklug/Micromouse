//Name: MM Maze - Build
//Original Author: Dillon Mills
//Editor: Robert Spiller
//Purpose: Randomly build a 16x16 maze to test MicroMouse algorithms.

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h> // for _getch()
#include <Windows.h>
#include "MazeGen.h"
#include "SensorPseudo.h"
#include "MotorPseudo.h"

void printFullMaze(int b[16][16][5]);
void printMouseMaze(int b[16][16][5], int, int, int, bool flood = false);
void mouseSearch(int b[16][16][5], int, int, int);
double distance(double, double, double, double);
void autoPilot(int b[16][16][5], int c[16][16][5], int d[16][16][5], int &, int &, int &, bool flood = false, bool discover = false, double targetrow = 7.5, double targetcol = 7.5);
void nearestUndiscovered(int c[16][16][5], int d[16][16][5], int , int , int &, int &);

void main() {
	int cell[16][16][5] = { 0 };	//maze and flood values
	
	printf("Generating...");
	randomMaze(cell);

	mouseSearch(cell, 0, 0, 0);
}

void printFullMaze(int b[16][16][5]) {
	//Print Full Maze//
	//prints visual representation of maze
	printf(" _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _");
	for (int i = 15; i >= 0; i--) {
		printf("\n|");
		for (int j = 0; j < 16; j++) {
			
			//visual representation
			if (b[i][j][2]) {
				printf("_");
			}
			else {
				printf(" ");
			}
			if (b[i][j][1]) {
				printf("|");
			}
			else {
				printf(" ");
			}
		}
	}
	printf("\n");
}

void printMouseMaze(int b[16][16][5], int row, int col, int dir, bool flood) {
	char wall[16] = {(char)32, (char)32, (char)32, (char)192, (char)32, (char)179, (char)218, (char)195, 
					 (char)32, (char)217, (char)196, (char)193, (char)191, (char)180, (char)194, (char)197};
	int n = 0;
	for (int i = 15; i >= 0; i--) {
		
		for (int j = 0; j < 16; j++) { // places ceiling of each row
			n = 0;
			if (i < 15) {
				n = b[i + 1][j][3];
			}
			if (j > 0) {
				n = n + 8*b[i][j - 1][0];
			}
			n = n + 2*b[i][j][0] + 4*b[i][j][3];
			printf("%c%c%c", wall[n], wall[10*b[i][j][0]], wall[10*b[i][j][0]]);
			if (j == 15) {
				n = 0;
				if (i < 15) {
					n = b[i + 1][j][1];
				}
				n = n + 4*b[i][j][1] + 8*b[i][j][0];
				printf("%c", wall[n]);
			}
		}
		printf("\n");

		for (int j = 0; j < 16; j++) { // places walls and spaces of each row
			printf("%c", wall[5*b[i][j][3]]);
			if (i == row && j == col) {
				switch (dir) {
					case 0: printf("/\\");	break;
					case 1: printf(">>");	break;
					case 2: printf("\\/");	break;
					case 3: printf("<<");	break;
					default: printf("XX");	break;
				}
			}
			else if (flood) {
				if (b[i][j][4] < 0 || b[i][j][4] > 9) {
					printf("%d", b[i][j][4]);
				}
				else {
					printf(" %d", b[i][j][4]);
				}
			}
			else {
				printf("  ");
			}
			if (j == 15) {
				printf("%c", wall[5*b[i][j][1]]);
			}
		}
		printf("\n");

		if (i == 0) {
			for (int j = 0; j < 16; j++) { // places floor of maze
				n = 0;
				if (j > 0) {
					n = n + 8*b[i][j - 1][2];
				}
				n = n + b[i][j][3] + 2*b[i][j][2];
				printf("%c%c%c", wall[n], wall[10*b[i][j][2]], wall[10*b[i][j][2]]);
				if (j == 15) {
					printf("%c", wall[b[i][j][1] + 8*b[i][j][2]]);
				}
			}
			printf("\n");
		}

	}
}

void mouseSearch(int b[16][16][5], int row, int col, int dir) {
	int c[16][16][5] = { 0 };	// mouse's personal wall array
	int d[16][16][5] = { 0 };
	int autodir = 0, autodirnum = 255, nextrow, nextcol;
	double tarrow = 7.5, tarcol = 7.5;
	double autodirdist = 0, nextdist = 0;
	char input = 0;
	bool flood = false;

	//----------//Constants//----------//
	//apply constants to the mouse wall array
	for (int i = 0; i < 16; i++) {
		c[0][i][2] = 1;
		d[0][i][2] = 1;
		c[15][i][0] = 1;
		d[15][i][0] = 1;
		c[i][0][3] = 1;
		d[i][0][3] = 1;
		c[i][15][1] = 1;
		d[i][15][1] = 1;
	}

	for (int i = 0; i < 4; i++) {
		d[0][0][i] = 1;
	}
	matchCells(d);

	while (true) {

		if (input == 'w' && !b[row][col][0]) {
			moveN(dir, row, col);
		}
		else if (input == 'd' && !b[row][col][1]) {
			moveE(dir, row, col);
		}
		else if (input == 's' && !b[row][col][2]) {
			moveS(dir, row, col);
		}
		else if (input == 'a' && !b[row][col][3]) {
			moveW(dir, row, col);
		}
		else if (input == 'f') {
			flood = !flood;
		}
		else if (input == 'g') {
			autoPilot(b, c, d, row, col, dir, flood);
		}
		else if (input == 'h') {
			for (int i = 0; i < 256; i++) {
				nearestUndiscovered(c, d, row, col, nextrow, nextcol);
				if (nextrow != -1 && nextcol != -1) {
					autoPilot(b, c, d, row, col, dir, flood, true, nextrow, nextcol);
				}
			}
		}
		else if (input == 'j') {
			autoPilot(b, c, d, row, col, dir, flood, false, 0, 0);
			dir = 0;
		}

		for (int i = 0; i < 4; i++) {
			d[row][col][i] = 1;
		}
		matchCells(d);

		senseWall(dir, row, col, b, c);
		matchCells(c);
		floodFill(c);

		system("cls");
		printFullMaze(b);
		printMouseMaze(c, row, col, dir, flood);
		printf("%d, %d\n%d", row, col, dir);
		//pause until key is pressed
		input = _getch();
	}
}

double distance(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void autoPilot(int b[16][16][5], int c[16][16][5], int d[16][16][5], int &row, int &col, int &dir, bool flood, bool discover, double targetrow, double targetcol) {
	int autodir, autodirnum;
	double autodirdist;
	bool tardisc = false;

	while (true) {

		if (targetrow == 7.5 && targetcol == 7.5) {
			floodFill(c);
		}
		else {
			floodFill(c, (int)targetrow, (int)targetcol);
		}

		if (discover) {
			tardisc = true;
			for (int i = 0; i < 4; i++) {
				if (!d[(int)targetrow][(int)targetcol][i]) {
					tardisc = false;
				}
			}
		}

		if (c[row][col][4] <= 0 || tardisc) { // if target has been reached or is closed off or all sides have been discover in discover-mode
			if (targetrow == 7.5 && targetcol == 7.5) { // if searching for center square, discover all the center cells
				for (int i = 7; i < 9; i++) {
					for (int j = 7; j < 9; j++) {
						for (int k = 0; k < 4; k++) {
							d[i][j][k] = 1;
						}
					}
				}
				for (int i = 7; i < 9; i++) {
					c[7][i][2] = 1;
					c[8][i][0] = 1;
					c[i][7][3] = 1;
					c[i][8][1] = 1;
				}
				c[row][col][(dir + 2)%4] = 0;
			}
			else {
				for (int i = 0; i < 4; i++) {
					d[(int)targetrow][(int)targetcol][i] = 1;
				}
			}
			matchCells(d);
			return;
		}

		autodir = -1;
		autodirnum = c[row][col][4];
		autodirdist = 100;
		if (row < 15) {
			if (c[row + 1][col][4] < autodirnum && !b[row][col][0] && distance(col, row + 1, targetcol, targetrow) < autodirdist) {
				autodir = 0;
				autodirdist = distance(col, row + 1, targetcol, targetrow);
			}
		}
		if (row > 0) {
			if (c[row - 1][col][4] < autodirnum && !b[row][col][2] && distance(col, row - 1, targetcol, targetrow) < autodirdist) {
				autodir = 2;
				autodirdist = distance(col, row - 1, targetcol, targetrow);
			}
		}
		if (col < 15) {
			if (c[row][col + 1][4] < autodirnum && !b[row][col][1] && distance(col + 1, row, targetcol, targetrow) < autodirdist) {
				autodir = 1;
				autodirdist = distance(col + 1, row, targetcol, targetrow);
			}
		}
		if (col > 0) {
			if (c[row][col - 1][4] < autodirnum && !b[row][col][3] && distance(col - 1, row, targetcol, targetrow) < autodirdist) {
				autodir = 3;
				autodirdist = distance(col - 1, row, targetcol, targetrow);
			}
		}
		switch (autodir) {
			case 0: moveN(dir, row, col); break;
			case 1: moveE(dir, row, col); break;
			case 2: moveS(dir, row, col); break;
			case 3: moveW(dir, row, col); break;
			default: break;
		}
		
		for (int i = 0; i < 4; i++) {
			d[row][col][i] = 1; // cell currently standing on is discovered
		}
		matchCells(d);

		senseWall(dir, row, col, b, c);
		matchCells(c);
		if (targetrow == 7.5 && targetcol == 7.5) {
			floodFill(c);
		}
		else {
			floodFill(c, (int)targetrow, (int)targetcol);
		}

		system("cls");
		printFullMaze(b);
		printMouseMaze(c, row, col, dir, flood);
		//printf("%d, %d\n%d", row, col, dir);
		//Sleep(250);

	}
}

void nearestUndiscovered(int c[16][16][5], int d[16][16][5], int row, int col, int &nextrow, int &nextcol) {
	int nextflood = 255;

	nextrow = -1;
	nextcol = -1;

	floodFill(c, row, col);
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			if (!d[i][j][0] || !d[i][j][1] || !d[i][j][2] || !d[i][j][3]) {
				if (c[i][j][4] < nextflood) {
					nextrow = i;
					nextcol = j;
					nextflood = c[i][j][4];
				}
			}
		}
	}
}
