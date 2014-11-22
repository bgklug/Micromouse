#ifndef SENSORPSEUDO_H_INCLUDED
#define SENSORPSEUDO_H_INCLUDED

int sensorReadL(int dir, const int & m[16][16][5], int row, int col);
int sensorReadF(int dir, const int & m[16][16][5], int row, int col);
int sensorReadR(int dir, const int & m[16][16][5], int row, int col);
void senseWall(int dir, int row, int col, const int & gen[16][16][5], int & mou[16][16][5]);


#endif