//Name: MM Maze - Build
//Original Author: Dillon Mills
//Editor: Robert Spiller
//Purpose: Randomly build a 16x16 maze to test MicroMouse algorithms.

/* 
 * w-a-s-d: movement
 * f: toggle flood output
 * g: make the mouse go to the center
 * h: make the mouse discovery all undiscovered cells
 * j: make the mouse return to origin
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h> // for _getch()
#include <Windows.h>
#include "MazeGen.h"
#include "SensorPseudo.h"
#include "MotorPseudo.h"
#include "WallSituations.h"

void printFullMaze(int b[16][16][5]);
void printMouseMaze(int b[16][16][5], int, int, int, bool flood = false);
void mouseSearch(int b[16][16][5], int, int, int);
double distance(double, double, double, double);
void autoPilot(int b[16][16][5], int c[16][16][5], int d[16][16][5], int &, int &, int &, bool flood = false, bool discover = false, double targetrow = 7.5, double targetcol = 7.5);
bool nearestUndiscovered(int c[16][16][5], int d[16][16][5], int , int , int &, int &);
void sampleMaze1(int b[16][16][5]);

void main() {
	int cell[16][16][5] = { 0 };	//maze and flood values
	
	printf("Generating...");
	randomMaze(cell);
	//sampleMaze1(cell);

	mouseSearch(cell, 0, 0, 0);
}

void printFullMaze(int b[16][16][5]) {
	//----------//Print Full Maze//----------//
	// Prints a succinct version of the full maze
	printf(" _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _");
	for (int i = 15; i >= 0; i--) {
		printf("\n|");
		for (int j = 0; j < 16; j++) {
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
	//----------//Print Mouse Maze//----------//
	// Prints the maze that the mouse sees
	// Analyzes what walls connect to each peg and prints the corners and walls of the maze
	// Also prints the mouse and its orientation, as well as the flood values if toggled on
	char wall[16] = {(char)32, (char)32, (char)32, (char)192, (char)32, (char)179, (char)218, (char)195, 
					 (char)32, (char)217, (char)196, (char)193, (char)191, (char)180, (char)194, (char)197};
	int n = 0;

	for (int i = 15; i >= 0; i--) {
		
		for (int j = 0; j < 16; j++) { // places ceiling of each row
			n = 0;
			if (i < 15) { // if not the top row, adds the north wall for each peg
				n = b[i + 1][j][3];
			}
			if (j > 0) { // if not the first column, adds the west wall for each peg
				n = n + 8*b[i][j - 1][0];
			}
			n = n + 2*b[i][j][0] + 4*b[i][j][3]; // sets n to be the number the corner represents
			printf("%c%c%c", wall[n], wall[10*b[i][j][0]], wall[10*b[i][j][0]]);
			if (j == 15) { // places far left wall
				n = 0;
				if (i < 15) { // if not the top row, adds north wall
					n = b[i + 1][j][1];
				}
				n = n + 4*b[i][j][1] + 8*b[i][j][0];
				printf("%c", wall[n]);
			}
		}
		printf("\n");

		for (int j = 0; j < 16; j++) { // places walls and spaces of each row
			printf("%c", wall[5*b[i][j][3]]); // prints wall if it exists
			if (i == row && j == col) { // if mouse is in the space, prints according to orientation
				switch (dir) {
					case 0: printf("/\\");	break;
					case 1: printf(">>");	break;
					case 2: printf("\\/");	break;
					case 3: printf("<<");	break;
					default: printf("XX");	break;
				}
			}
			else if (flood) { // if flood is toggled on, prints the flood value
				if (b[i][j][4] < 0 || b[i][j][4] > 9) { // if value takes up two spaces, don't print a space before
					printf("%d", b[i][j][4]);
				}
				else {
					printf(" %d", b[i][j][4]);
				}
			}
			else { // else, prints a space
				printf("  ");
			}
			if (j == 15) {
				printf("%c", wall[5*b[i][j][1]]);
			}
		}
		printf("\n");

		if (i == 0) { // if last row, prints the floor as well
			for (int j = 0; j < 16; j++) { // places floor of maze
				n = 0;
				if (j > 0) {
					n = n + 8*b[i][j - 1][2];
				}
				n = n + b[i][j][3] + 2*b[i][j][2];
				printf("%c%c%c", wall[n], wall[10*b[i][j][2]], wall[10*b[i][j][2]]);
				if (j == 15) { // if last column, prints bottom right corner
					printf("%c", wall[b[i][j][1] + 8*b[i][j][2]]);
				}
			}
			printf("\n");
		}

	}
}

void mouseSearch(int b[16][16][5], int row, int col, int dir) {
	int c[16][16][5] = { 0 };	// mouse's personal wall array
	int d[16][16][5] = { 0 };	// Discovery array (5 values per cell so it can fit in the 'matchCells' function
	int autodir = 0, autodirnum = 255, nextrow, nextcol;
	double tarrow = 7.5, tarcol = 7.5;
	double autodirdist = 0, nextdist = 0;
	char input = 0;
	bool flood = false;

	//----------//Constants//----------//
	// Apply outer wall constants to the mouse wall array and the discovery array
	/*for (int i = 0; i < 16; i++) {
		c[0][i][2] = 1;
		d[0][i][2] = 1;
		c[15][i][0] = 1;
		d[15][i][0] = 1;
		c[i][0][3] = 1;
		d[i][0][3] = 1;
		c[i][15][1] = 1;
		d[i][15][1] = 1;
	}*/

	// Discovers the front 3 opening cell walls
	/*for (int i = 0; i < 4; i++) {
		if (i != dir) {
			d[row][col][i] = 1;
		}
	}
	matchCells(d);*/

	for (int i = 0; i < 4; i++) { // simulates mouse turning in place to discover origin cell
		d[row][col][i] = 1;
		c[row][col][i] = b[row][col][i];
	}

	while (true) { // infinite input loop

		if (input == 'w' && !b[row][col][0]) { // if 'w,' go north
			moveN(dir, row, col);
		}
		else if (input == 'd' && !b[row][col][1]) { // if 'd,' go east
			moveE(dir, row, col);
		}
		else if (input == 's' && !b[row][col][2]) { // if 's,' go south
			moveS(dir, row, col);
		}
		else if (input == 'a' && !b[row][col][3]) { // if 'a,' go west
			moveW(dir, row, col);
		}
		else if (input == 'f') { // if 'f,' toggle displaying flood values
			flood = !flood;
		}
		else if (input == 'g') { // if 'g,' go to the center
			autoPilot(b, c, d, row, col, dir, flood);
		}
		else if (input == 'h') { // if 'h,' go to the nearest undiscovered cell
			for (int i = 0; i < 256; i++) { // total number of cells
				if (nearestUndiscovered(c, d, row, col, nextrow, nextcol)) { // if there is a valid nextrow and nextcol to go to...
					autoPilot(b, c, d, row, col, dir, flood, true, nextrow, nextcol); // ... go to it
				}
			}
		}
		else if (input == 'j') { // if 'j,' go to the south-west corner
			autoPilot(b, c, d, row, col, dir, flood, false, 0, 0);
			dir = 0;
		}
		else if (input == 'q') { // if 'q,' quit
			return;
		}

		for (int i = 0; i < 4; i++) { // discovers the walls not behind the mouse in its current cell
			if (i != (dir + 2)%4) {
				d[row][col][i] = 1;
			}
		}
		matchCells(d);

		senseWall(dir, row, col, b, c); // place walls the mouse can sense in its current cell and orientation
		matchCells(c);
		floodFill(c); // floods the maze in its current state

		system("cls"); // clears the screen
		printFullMaze(b); // prints the full maze
		printMouseMaze(c, row, col, dir, flood); // prints the maze the mouse can see
		printf("%d, %d\n%d", row, col, dir); // prints the current coordinates and orientation
		//pause until key is pressed
		input = _getch(); // waits for input and stores the 'char' values
	}
}

double distance(double x1, double y1, double x2, double y2) { // returns the distance between two coordinates
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void autoPilot(int b[16][16][5], int c[16][16][5], int d[16][16][5], int &row, int &col, int &dir, bool flood, bool discover, double targetrow, double targetcol) {
	int autodir, autodirnum;
	double autodirdist;
	bool tardisc = false;

	while (true) {

		// Floodfills mouse's maze array from different locations depending on the target
		if (targetrow == 7.5 && targetcol == 7.5) {
			floodFill(c);
		}
		else {
			floodFill(c, (int)targetrow, (int)targetcol);
		}

		// If in discovery mode, signals function return when target cell is fully discovered
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
							d[i][j][k] = 1; // discover all cell walls
							c[i][j][k] = b[i][j][k]; // set walls of center equal to actual maze walls
						}
					}
				}
			}
			for (int i = 0; i < 4; i++) { // discovers final cell
				d[(int)targetrow][(int)targetcol][i] = 1;
			}
			matchCells(d);
			return;
		}

		autodir = -1;
		autodirnum = c[row][col][4];
		autodirdist = 100;
		if (row < 15) {
			// If cell above has a lower flood value and is accessible and is closer to the target than the previous checked cell
			if (c[row + 1][col][4] < autodirnum && !b[row][col][0] && distance(col, row + 1, targetcol, targetrow) < autodirdist) {
				autodir = 0;
				autodirdist = distance(col, row + 1, targetcol, targetrow);
			}
		}
		if (row > 0) {
			// If cell below ... (same as above)
			if (c[row - 1][col][4] < autodirnum && !b[row][col][2] && distance(col, row - 1, targetcol, targetrow) < autodirdist) {
				autodir = 2;
				autodirdist = distance(col, row - 1, targetcol, targetrow);
			}
		}
		if (col < 15) {
			// If cell to the right ... (same as the first)
			if (c[row][col + 1][4] < autodirnum && !b[row][col][1] && distance(col + 1, row, targetcol, targetrow) < autodirdist) {
				autodir = 1;
				autodirdist = distance(col + 1, row, targetcol, targetrow);
			}
		}
		if (col > 0) {
			// If cell to the left ... (same as the first)
			if (c[row][col - 1][4] < autodirnum && !b[row][col][3] && distance(col - 1, row, targetcol, targetrow) < autodirdist) {
				autodir = 3;
				autodirdist = distance(col - 1, row, targetcol, targetrow);
			}
		}
		switch (autodir) { // if a valid move has been determined, move there based on its direction
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

		// Prints the maze(s) to the screen
		/*system("cls");
		printFullMaze(b);
		printMouseMaze(c, row, col, dir, flood);
		if (discover) {
			printf("Searching...\n");
		}
		else {
			printf("Going to: %.0f, %.0f", targetrow, targetcol);
		}*/
		//Sleep(250);

	}
}

bool nearestUndiscovered(int c[16][16][5], int d[16][16][5], int row, int col, int &nextrow, int &nextcol) {
	int nextflood = 255;
	int origrow = nextrow, origcol = nextcol;

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

	if (nextrow == -1 || nextcol == -1) {
		nextrow = origrow;
		nextcol = origcol;
		return false;
	}
	return true;
}

void sampleMaze1(int b[16][16][5]) {
	char c[16][16] = {	{	12,	12,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	6	},
						{	8,	8,	4,	4,	4,	8,	12,	0,	12,	4,	4,	4,	4,	4,	4,	10	},
						{	8,	0,	12,	4,	0,	12,	0,	12,	0,	12,	4,	4,	4,	4,	0,	10	},
						{	12,	0,	12,	4,	8,	8,	12,	0,	12,	0,	4,	4,	4,	4,	8,	10	},
						{	8,	8,	8,	4,	0,	8,	0,	12,	0,	12,	0,	8,	4,	8,	8,	10	},
						{	8,	8,	8,	12,	4,	4,	4,	0,	12,	0,	8,	4,	8,	0,	8,	10	},
						{	8,	8,	8,	12,	4,	8,	12,	4,	0,	12,	0,	8,	4,	8,	8,	10	},
						{	8,	8,	8,	8,	8,	8,	8,	12,	4,	8,	4,	8,	0,	8,	8,	10	},
						{	8,	12,	0,	8,	8,	8,	8,	8,	0,	12,	0,	8,	12,	0,	8,	2	},
						{	8,	0,	4,	8,	8,	8,	8,	4,	8,	0,	12,	0,	8,	4,	4,	10	},
						{	8,	12,	0,	8,	0,	8,	4,	0,	4,	4,	0,	8,	8,	12,	8,	10	},
						{	8,	8,	12,	0,	8,	8,	4,	4,	4,	4,	4,	0,	0,	8,	0,	10	},
						{	8,	8,	0,	8,	4,	0,	4,	4,	4,	4,	4,	4,	4,	4,	8,	10	},
						{	8,	12,	0,	4,	12,	4,	4,	4,	4,	8,	12,	4,	4,	0,	8,	10	},
						{	8,	8,	12,	0,	8,	4,	4,	4,	8,	8,	4,	4,	4,	0,	8,	10	},
						{	9,	1,	9,	5,	5,	5,	5,	5,	1,	1,	5,	5,	5,	5,	1,	3	}
					};
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			for (int k = 0; k < 4; k++) {
				b[i][j][k] = (int)wallExists(c[i][j], (char)k);
			}
		}
	}
	matchCells(b);
}
