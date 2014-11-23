#include <stdio.h>
#include <stdlib.h>		//for rand() and srand()
#include <time.h>		//for time()
#include "MazeGen.h"

void randomMaze(int b[16][16][5]) {
	int goodGen = 0;	//whether or not the maze is solvable
	srand((unsigned int)time(NULL));
	
	//----------//----------//MAZE GENERATION//----------//----------//
	
	while (!goodGen) {	//makes sure maze is solvable
		clearWalls(b);
		generateRandomMaze(b);
		goodGen = floodFill(b);
	}
}

void clearWalls(int b[16][16][5]) {
	//Clean Slate//
	//starts the maze without having any walls anywhere
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			for (int k = 0; k < 4; k++) {
				b[i][j][k] = 0;
			}
		}
	}
}

void generateRandomMaze(int b[16][16][5]) {
	int percent = 50; // percent chance of placing a wall
	//----------//Walls//----------//
	//build a random maze
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			//randomize the bottom and left sides of each cell
			for (int k = 2; k < 4; k++) {
				b[i][j][k] = (int)(rand()%100 < percent);
			}
		}
	}

	//----------//Constants//----------//
	//After the maze is randomly generated, these constants are placed
	
	//Center Walls//
	//makes the center squares have walls on all sides
	for (int i = 7; i <= 8; i++) {
		for (int j = 7; j <= 8; j++) {
			for (int k = 0; k < 4; k++) {
				b[i][j][k] = 1;
			}
		}
	}

	//Center Opening//
	//places center wall opening
	switch (rand()%8) {
		case 0: b[7][7][3] = 0; break;
		case 1: b[7][7][2] = 0; break;
		case 2: b[7][8][2] = 0; break;
		case 3: b[7][8][1] = 0; break;
		case 4: b[8][8][1] = 0; break;
		case 5: b[8][8][0] = 0; break;
		case 6: b[8][7][0] = 0; break;
		case 7: b[8][7][3] = 0; break;
	}

	//Border//
	//make the edges have walls
	for (int i = 0; i < 16; i++) {
		b[0][i][2] = 1;
		b[15][i][0] = 1;
		b[i][0][3] = 1;
		b[i][15][1] = 1;
	}
	
	//Corners//
	//places the starting positions in the four corners
	b[0][0][0] = 0;
	b[0][0][1] = 1;
	b[0][0][2] = 1;
	b[0][0][3] = 1;
	b[0][1][3] = 1;
	b[1][0][2] = 0;
	b[0][15][0] = 1;
	b[0][15][1] = 1;
	b[0][15][2] = 1;
	b[0][15][3] = 0;
	b[0][14][1] = 0;
	b[1][15][2] = 1;
	b[15][0][0] = 1;
	b[15][0][1] = 0;
	b[15][0][2] = 1;
	b[15][0][3] = 1;
	b[15][1][3] = 0;
	b[14][0][0] = 1;
	b[15][15][0] = 1;
	b[15][15][1] = 1;
	b[15][15][2] = 0;
	b[15][15][3] = 1;
	b[15][14][1] = 1;
	b[14][15][0] = 0;

	matchCells(b);

	//Pegs//
	//makes sure a wall is on every peg
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			if ((i > 0) && (j > 0)) {
				if (!(b[i][j][3] + b[i][j][2] + b[i - 1][j - 1][0] + b[i - 1][j - 1][1])) {
					switch (rand()%4) {
						case 0: b[i][j][3] = 1;			break;
						case 1: b[i][j][2] = 1;			break;
						case 2: b[i - 1][j - 1][0] = 1;	break;
						case 3: b[i - 1][j - 1][1] = 1;	break;
					}
				}
			}
		}
	}

	matchCells(b);
}

void matchCells(int b[16][16][5]) {
	//Matching//
	//makes sure each cell has the same walls as the adjacent cells
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			if (i > 0) {
				if (b[i - 1][j][0]) {
					b[i][j][2] = 1;
				}
				else if (b[i][j][2]) {
					b[i - 1][j][0] = 1;
				}
			}
			if (i < 15) {
				if (b[i + 1][j][2]) {
					b[i][j][0] = 1;
				}
				else if (b[i][j][0]) {
					b[i + 1][j][2] = 1;
				}
			}
			if (j > 0) {
				if (b[i][j - 1][1]) {
					b[i][j][3] = 1;
				}
				if (b[i][j][3]) {
					b[i][j - 1][1] = 1;
				}
			}
			if (j < 15) {
				if (b[i][j + 1][3]) {
					b[i][j][1] = 1;
				}
				if (b[i][j][1]) {
					b[i][j + 1][3] = 1;
				}
			}
		}
	}
}

bool floodFill(int b[16][16][5], int row, int col) {
	//----------//Flood//----------//
	//fills all flood array spaces with -1
	for(int i = 0; i < 16; i++) {
		for(int j = 0; j < 16; j++) {
			b[i][j][4] = -1;
		}
	}

	//fills the four goal flood array spaces with 0 by default
	if (row < 0 || col < 0) {
		b[7][7][4] = 0;
		b[7][8][4] = 0;
		b[8][7][4] = 0;
		b[8][8][4] = 0;
	}
	else {
		b[row][col][4] = 0;
	}

	//fills the flood array with values using flood fill logic
	for (int k = 0; k < 256; k++) {
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 16; j++) {
				if (b[i][j][4] == k) {  //if the flood array space equals k (starting at 0), place k+1 in adjacent flood array spaces
					if (i < 15) {
						if (!b[i + 1][j][2] && b[i + 1][j][4] == -1) {  //North
							b[i + 1][j][4] = b[i][j][4] + 1;
						}
					}
					if (i > 0) {
						if (!b[i - 1][j][0] && b[i - 1][j][4] == -1) {  //South
							b[i - 1][j][4] = b[i][j][4] + 1;
						}
					}
					if (j < 15) {
						if (!b[i][j + 1][3] && b[i][j + 1][4] == -1) {  //East
							b[i][j + 1][4] = b[i][j][4] + 1;
						}
					}
					if (j > 0) {
						if(!b[i][j - 1][1] && b[i][j - 1][4] == -1) {  //West
							b[i][j - 1][4] = b[i][j][4] + 1;
						}
					}
				}
			}
		}
	}
	
	//Check//
	/*
	//optional: makes sure all cells are discoverable
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			if (c[i][j] == -1) {
				return 0;
			}
		}
	}
	*/
	//checks if all four corners can reach the center
	if (b[0][0][4] != -1 && b[0][15][4] != -1 && b[15][0][4] != -1 && b[15][15][4] != -1) {
		return 1;
	}
	return 0;
}

/* Functions for future reference

prints the maze with the option of displaying the flood values or tracing the fastests path(s)

void printMaze(int b[16][16][5], int d[16][16], bool flood, bool bestPath) {
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
			if (bestPath) {
				if (d[i][j]) {
					printf(" %c", wall[d[i][j]]);
				}
				else {
					printf("  ");
				}
			}
			else if (flood) {
				if (b[i][j][4] < 10 && b[i][j][4] > -1) {
					printf(" %d", b[i][j][4]);
				}
				else {
					printf("%d", b[i][j][4]);
				}
			}
			else {
				printf("  ");
			}
			if (j == 15) {
				printf("%c", wall[5]);
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
				printf("%c%c%c", wall[n], wall[10], wall[10]);
				if (j == 15) {
					printf("%c", wall[9]);
				}
			}
			printf("\n");
		}

	}
}

finds the best path and puts values into 'd' that will be interpreted as walls later

void bestPath(int b[16][16][5], int d[16][16]) {
	int dir = 0;

	// starts the path at the bottom left corner
	d[0][0] = 4;

	for (int k = 255; k >= 0; k--) {
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 16; j++) {
				if (b[i][j][4] == k) {  //if the flood array space equals k (starting at 0), place k+1 in adjacent flood array spaces
					if (i < 15) {
						if (!b[i + 1][j][2] && b[i + 1][j][4] == k + 1 && d[i + 1][j]) {  //North
							d[i][j] = 1;
							d[i + 1][j] = d[i + 1][j] + 4;
						}
					}
					if (i > 0) {
						if (!b[i - 1][j][0] && b[i - 1][j][4] == k + 1 && d[i - 1][j]) {  //South
							d[i][j] = 4;
							d[i - 1][j] = d[i - 1][j] + 1;
						}
					}
					if (j < 15) {
						if (!b[i][j + 1][3] && b[i][j + 1][4] == k + 1 && d[i][j + 1]) {  //East
							d[i][j] = 2;
							d[i][j + 1] = d[i][j + 1] + 8;
						}
					}
					if (j > 0) {
						if(!b[i][j - 1][1] && b[i][j - 1][4] == k + 1 && d[i][j - 1]) {  //West
							d[i][j] = 8;
							d[i][j - 1] = d[i][j - 1] + 2;
						}
					}
				}
			}
		}
	}
}
*/
