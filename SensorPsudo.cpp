#include "SensorPsudo.h"
int sensorReadL(int dir, const int & m[16][16][5], int row, int col){
	return m[row][col][(dir - 1)% 4];//will return 1 for wall or 0 for space
}
int sensorReadF(int dir, const int & m[16][16][5], int row, int col){
	return m[row][col][dir];
}
int sensorReadR(int dir, const int & m[16][16][5], int row, int col){
	return m[row][col][(dir + 1)% 4];
}
void senseWall(int dir, int row, int col, const int & gen[16][16][5], int & mou[16][16][5]){
	mou[row][col][(dir-1)%4] = sensorReadL(dir, gen, row, col);
	mou[row][col][dir] = sensorReadF(dir, gen, row, col);
	mou[row][col][(dir+1)%4] = sensorReadR(dir, gen, row, col);
}