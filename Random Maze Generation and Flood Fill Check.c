//Name: MM Maze - Build
//Original Author: Dillon Mills
//Editor: Robert Spiller
//Purpose: Randomly build a 16x16 maze to test MicroMouse algorithms.

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h> // for _getch()

void clearArray(int b[16][16][4]);
void generateRandomMaze(int b[16][16][4]);
void matchCells(int b[16][16][4]);
bool floodFill(int b[16][16][4], int c[16][16]);
void printMaze(int b[16][16][4]);
void printMaze2(int b[16][16][4], int c[16][16], int d[16][16], bool flood = false, bool bestPath = false);
void printFloodFill(int c[16][16]);
void bestPath(int b[16][16][4], int c[16][16], int d[16][16]);

//----------//----------//NOTES//----------//----------//

void main() {
	//----------//----------//INITIALIZATION//----------//----------//
	int b[16][16][4] = { 0 };	//maze
	int c[16][16] = { 0 };		//flood (consider combining with maze b[][][])
	int d[16][16] = { 0 };
	int gen = 0;				//generation count
	int goodGen = 0;			//whether or not the maze is solvable
	clock_t tStart = clock(), tEnd = clock();	//starting and ending time
	srand((unsigned int)time(NULL));
	
	//----------//----------//MAZE GENERATION//----------//----------//
	
	while (!goodGen) {	//makes sure maze is solvable
		
		clearArray(b);
		generateRandomMaze(b);
		goodGen = floodFill(b, c);
		
		//----------//Debugging//----------//
		//code to help debug
	
		//Print Info//
		//prints time elapsed, generation count, and goodSeed value to console
		system("cls");
		tEnd = clock();
		printf("Time:\t\t%.2f\n", (double)(tEnd - tStart)/1000);
		printf("Generation:\t%d\n", gen++);
		//printMaze(b);_getch();
	}
	bestPath(b, c, d);
	printMaze(b);
	printf("\n\n");
	/*printFloodFill(c);
	printf("\n");*/
	printMaze2(b, c, d, true, true);
}

void clearArray(int b[16][16][4]) {
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

void generateRandomMaze(int b[16][16][4]) {
	int percent = 33; // percent chance of placing a wall
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

void matchCells(int b[16][16][4]) {
	//Matching//
	//makes sure each cell has the same walls as the adjacent cells
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			if (i > 0 && b[i - 1][j][0]) {
				b[i][j][2] = 1;
			}
			if (i < 15 && b[i + 1][j][2]) {
				b[i][j][0] = 1;
			}
			if (j > 0 && b[i][j - 1][1]) {
				b[i][j][3] = 1;
			}
			if (j < 15 && b[i][j + 1][3]) {
				b[i][j][1] = 1;
			}
		}
	}
}

bool floodFill(int b[16][16][4], int c[16][16]) {
	//----------//Flood//----------//
	//fills all flood array spaces with -1
	for(int i = 0; i < 16; i++) {
		for(int j = 0; j < 16; j++) {
			c[i][j] = -1;
		}
	}

	//fills the four goal flood array spaces with 0
	c[7][7] = 0;
	c[7][8] = 0;
	c[8][7] = 0;
	c[8][8] = 0;

	//fills the flood array with values using flood fill logic
	for (int k = 0; k < 256; k++) {
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 16; j++) {
				if (c[i][j] == k) {  //if the flood array space equals k (starting at 0), place k+1 in adjacent flood array spaces
					if (i < 15) {
						if (!b[i + 1][j][2] && (c[i + 1][j] == -1)) {  //North
							c[i + 1][j] = c[i][j] + 1;
						}
					}
					if (i > 0) {
						if (!b[i - 1][j][0] && (c[i - 1][j] == -1)) {  //South
							c[i - 1][j] = c[i][j] + 1;
						}
					}
					if (j < 15) {
						if (!b[i][j + 1][3] && (c[i][j + 1] == -1)) {  //East
							c[i][j + 1] = c[i][j] + 1;
						}
					}
					if (j > 0) {
						if(!b[i][j - 1][1] && (c[i][j - 1] == -1)) {  //West
							c[i][j - 1] = c[i][j] + 1;
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
	if ((c[0][0] != -1) && (c[0][15] != -1) && (c[15][0] != -1) && (c[15][15] != -1)) {
		return 1;
	}
	return 0;
}

void printMaze(int b[16][16][4]) {
	//Print Maze//
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

			//printf("%d\t",c[i][j]);	//flood value
		}
	}
}

/*char wall[16] = {(char)32, (char)32, (char)32, (char)192, (char)32, (char)179, (char)218, (char)195, 
					 (char)32, (char)217, (char)196, (char)193, (char)191, (char)180, (char)194, (char)197};*/

void printMaze2(int b[16][16][4], int c[16][16], int d[16][16], bool flood, bool bestPath) {
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
				if (c[i][j] < 10 && c[i][j] > -1) {
					printf(" %d", c[i][j]);
				}
				else {
					printf("%d", c[i][j]);
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

double distance(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void printFloodFill(int c[16][16]) {
	for (int i = 15; i >= 0; i--) {
		for (int j = 0; j < 16; j++) {
			if (c[i][j] < 0 || c[i][j] > 9) {
				printf(" %d", c[i][j]);
			}
			else {
				printf("  %d", c[i][j]);
			}
		}
		printf("\n");
	}
}

void bestPath(int b[16][16][4], int c[16][16], int d[16][16]) {
	int dir = 0;

	// starts the path at the bottom left corner
	d[0][0] = 4;

	for (int k = 255; k >= 0; k--) {
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 16; j++) {
				if (c[i][j] == k) {  //if the flood array space equals k (starting at 0), place k+1 in adjacent flood array spaces
					if (i < 15) {
						if (!b[i + 1][j][2] && c[i + 1][j] == k + 1 && d[i + 1][j]) {  //North
							d[i][j] = 1;
							d[i + 1][j] = d[i + 1][j] + 4;
						}
					}
					if (i > 0) {
						if (!b[i - 1][j][0] && c[i - 1][j] == k + 1 && d[i - 1][j]) {  //South
							d[i][j] = 4;
							d[i - 1][j] = d[i - 1][j] + 1;
						}
					}
					if (j < 15) {
						if (!b[i][j + 1][3] && c[i][j + 1] == k + 1 && d[i][j + 1]) {  //East
							d[i][j] = 2;
							d[i][j + 1] = d[i][j + 1] + 8;
						}
					}
					if (j > 0) {
						if(!b[i][j - 1][1] && c[i][j - 1] == k + 1 && d[i][j - 1]) {  //West
							d[i][j] = 8;
							d[i][j - 1] = d[i][j - 1] + 2;
						}
					}
				}
			}
		}
	}
}
