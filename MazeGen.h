#ifndef MAZEGEN_H
#define MAZEGEN_H

void randomMaze(int b[16][16][5]);
void clearWalls(int b[16][16][5]);
void generateRandomMaze(int b[16][16][5]);
void matchCells(int b[16][16][5]);
bool floodFill(int b[16][16][5], int row = -1, int col = -1);

#endif
