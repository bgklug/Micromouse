#ifndef MOTORPSEUDO_H_INCLUDED
#define MOTORPSEUDO_H_INCLUDED

void moveN(int & dir, int & mouRow, int & mouCol);
void moveE(int & dir, int & mouRow, int & mouCol);
void moveS(int & dir, int & mouRow, int & mouCol);
void moveW(int & dir, int & mouRow, int & mouCol);

void moveF(int dir, int & mouRow, int & mouCol);
void moveB();
void turnR(int & dir);
void turnL(int & dir);

#endif