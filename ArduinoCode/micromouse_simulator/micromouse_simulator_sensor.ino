char sensorReadL(byte dir, const char m[SIZE][SIZE][5], byte row, byte col){
	return m[row][col][(dir + 3)%4];//will return 1 for wall or 0 for space
}
char sensorReadF(byte dir, const char m[SIZE][SIZE][5], byte row, byte col){
	return m[row][col][dir];
}
char sensorReadR(byte dir, const char m[SIZE][SIZE][5], byte row, byte col){
	return m[row][col][(dir + 1)%4];
}
void senseWall(byte dir, byte row, byte col, const char gen[SIZE][SIZE][5], char mou[SIZE][SIZE][5]){
	mou[row][col][(dir + 3)%4] = sensorReadL(dir, gen, row, col);
	mou[row][col][dir] = sensorReadF(dir, gen, row, col);
	mou[row][col][(dir + 1)%4] = sensorReadR(dir, gen, row, col);
//	Serial.println("Sense Walls");
//	Serial.print((int)mou[row][col][(dir + 3)%4]);
//	Serial.print((int)mou[row][col][dir%4]);
//	Serial.println((int)mou[row][col][(dir + 1)%4]);
}
