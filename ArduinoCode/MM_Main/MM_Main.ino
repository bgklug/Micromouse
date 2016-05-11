#define SIZE  16      // Size of the mouse maze.
#define UCEN  SIZE/2  // Upper cell value that corresponds to goal cell locations.
#define LCEN  UCEN-1  // Lower cell value that corresponds to goal cell locations.
#define RPM   255     // Motor default run speed.

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *motorL = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *motorR = AFMS.getStepper(200, 1);

char m[SIZE][SIZE];      // Mouse maze wall values.
char f[SIZE][SIZE];      // Mouse maze flood values (needs +-).
boolean p[SIZE][SIZE];   // Best path detection array (mouse remembers best path).
boolean pchange;         // If mouse changes its path.
byte row = 0;            // Mouse global row position value.
byte col = 0;            // Mouse global column position value.
byte dir = 2;            // Mouse direction/orientation (NESW).
boolean select = false;  // Selecter switch for diagonal sensors.


void setup() {
  // begin serial communications at 9600 bits of data per second
  // for communicating with the computer, use one of these rates: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200
  Serial.begin(9600);
  
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);

  AFMS.begin();
  motorL->setSpeed(RPM);
  motorR->setSpeed(RPM);

  delay(5000);
  
  startWalls();
  calibrateSensors(dir);
} // end setup


void loop(){
  Serial.println("----- Finding Center -----");
  autoPilot(row, col, dir, UCEN, UCEN, true);
  Serial.println("----- Returning Home -----");
  autoPilot(row, col, dir, 0, 0, false);
}


//THIS FUNCTION FUCKS WITH THE MOTORS FOR SOME REASON!
void startWalls(){
  m[0][0] = 14;
  //m[0][SIZE-1] = 7;
  //m[SIZE-1][0] = 13;
  //m[SIZE-1][SIZE-1] = 11;
  
  m[LCEN][LCEN] = 3;
  m[LCEN][UCEN] = 9;
  m[UCEN][LCEN] = 6;
  m[UCEN][UCEN] = 12;
} // end startWalls

void centerWalls(byte row, byte col, byte dir){
  m[LCEN][LCEN] = 15;
  m[LCEN][UCEN] = 15;
  m[UCEN][LCEN] = 15;
  m[UCEN][UCEN] = 15;
  wallChange(m[row][col], (dir+2)%4, 0);
} // end centerWalls


boolean floodFill(int row, int col) {
  //----------//Flood//----------//
  //fills all flood array spaces with -1
  for(byte i = 0; i < SIZE; i++) {
    for(byte j = 0; j < SIZE; j++) {
      f[i][j] = -1;
    }
  }
  //fills the four goal flood array spaces with 0 by default
  if (row < 0 || col < 0) {
    f[LCEN][LCEN] = 0;
    f[LCEN][UCEN] = 0;
    f[UCEN][LCEN] = 0;
    f[UCEN][UCEN] = 0;
  }
  else {
    f[row][col] = 0;
  }
  //fills the flood array with values using flood fill logic
  for (int k = 0; k < SIZE*SIZE; k++) {  //byte's range is 0-255, so int will stop infinite loop from being created
    for (byte i = 0; i < SIZE; i++) {
      for (byte j = 0; j < SIZE; j++) {
        if (f[i][j] == k) {  //if the flood array space equals k (starting at 0), place k+1 in adjacent flood array spaces
          if (i < SIZE - 1) {
            if (!wallExists(m[i][j],0) && f[i + 1][j] == -1) {  //North
              f[i + 1][j] = f[i][j] + 1;
            }
          }
          if (i > 0) {
            if (!wallExists(m[i][j],2) && f[i - 1][j] == -1) {  //South
              f[i - 1][j] = f[i][j] + 1;
            }
          }
          if (j < SIZE - 1) {
            if (!wallExists(m[i][j],1) && f[i][j + 1] == -1) {  //East
              f[i][j + 1] = f[i][j] + 1;
            }
          }
          if (j > 0) {
            if(!wallExists(m[i][j],3) && f[i][j - 1] == -1) {  //West
              f[i][j - 1] = f[i][j] + 1;
            }
          }
        }
      }
    }
  }
  //checks if all four corners can reach the center
  if (f[0][0] != -1 && f[0][SIZE - 1] != -1 && f[SIZE - 1][0] != -1 && f[SIZE - 1][SIZE - 1] != -1) {
    return 1;
  }
  return 0;
} // end floodFill


void matchCells() {
  //Matching//
  //makes sure each cell has the same walls as the adjacent cells
  for (byte i = 0; i < SIZE; i++) {
    for (byte j = 0; j < SIZE; j++) {
      if (i > 0) {
        if (wallExists(m[i-1][j],0)) {
          wallChange(m[i][j],2,1);
        }
        else if (wallExists(m[i][j],2)) {
          wallChange(m[i-1][j],0,1);
        }
      }
      if (i < SIZE-1) {
        if (wallExists(m[i+1][j],2)) {
          wallChange(m[i][j],0,1);
        }
        else if (wallExists(m[i][j],0)) {
          wallChange(m[i+1][j],2,1);
        }
      }
      if (j > 0) {
        if (wallExists(m[i][j-1],1)) {
          wallChange(m[i][j],3,1);
        }
        else if (wallExists(m[i][j],3)) {
          wallChange(m[i][j-1],1,1);
        }
      }
      if (j < SIZE-1) {
        if (wallExists(m[i][j+1],3)) {
          wallChange(m[i][j],1,1);
        }
        else if (wallExists(m[i][j],1)) {
          wallChange(m[i][j+1],3,1);
        }
      }
    }
  }
} // end matchCells


void autoPilot(byte &row, byte &col, byte &dir, float targetrow, float targetcol, boolean center) {
  senseWall(m[row][col], dir, row, col); // senses the initial cell's walls (initialized now).
  matchCells();
  p[row][col] = true; // cell currently standing in is part of the path
  pchange = false;
  while (true) {
    
    // Floodfills mouse's maze array from different locations depending on the target
    if ((targetrow == UCEN || targetrow == LCEN) && (targetcol == UCEN || targetcol == LCEN)) {
      floodFill(-1, -1);
    }
    else {
      floodFill((int)targetrow, (int)targetcol);
    }
    
    if (f[row][col] == 0 && center) { // if target has been reached...
      centerWalls(row,col,dir);
      return;
    }else if(f[row][col] <= 0){ // ...or is closed off...
      return; 
    }
    
    switch (autoDirection()) { // if a valid move has been determined, move there based on its direction
      case 0: moveN(dir, row, col); break;
      case 1: moveE(dir, row, col); break;
      case 2: moveS(dir, row, col); break;
      case 3: moveW(dir, row, col); break;
      default: while(1){turnL(dir);} break;
    }
    
    senseWall(m[row][col], dir, row, col);
    
    if(wallExists(m[row][col], dir)){
      centerF();}
    
    matchCells();
    
    if (p[row][col] == false) { // if never been on this cell before, indicate path change
      pchange = true;
      p[row][col] = true;
    }
    //printFullMaze(m,row,col);
  }
} // end autoPilot


byte autoDirection (){
  byte autodir = -1;
  byte fprox[5];
  if (row < SIZE-1 && !wallExists(m[row][col],0)) {fprox[0] = f[row+1][col];} else {fprox[0] = 255;}
  if (col < SIZE-1 && !wallExists(m[row][col],1)) {fprox[1] = f[row][col+1];} else {fprox[1] = 255;}
  if (row > 0 && !wallExists(m[row][col],2)) {fprox[2] = f[row-1][col];} else {fprox[2] = 255;}
  if (col > 0 && !wallExists(m[row][col],3)) {fprox[3] = f[row][col-1];} else {fprox[3] = 255;}
  fprox[4] = min(min(fprox[0],fprox[1]),min(fprox[2],fprox[3]));
  
  if (row < SIZE-1 && fprox[0] == fprox[4] && !wallExists(m[row][col],0)){
  switch (dir) {
    case 0: return 0;
    case 2: if(fprox[0] < fprox[1] && fprox[0] < fprox[3]){return 0;} break;
    default: autodir = 0; break;
    }
  }
  if (col < SIZE-1 && fprox[1] == fprox[4] && !wallExists(m[row][col],1)){
    switch (dir) {
    case 1: return 1;
    case 3: if(fprox[1] < fprox[0] && fprox[1] < fprox[2]){return 1;} break;
    default: autodir = 1; break;
    }
  }
  if (row > 0 && fprox[2] == fprox[4] && !wallExists(m[row][col],2)){
    switch (dir) {
    case 0: if(fprox[2] < fprox[1] && fprox[2] < fprox[3]){return 2;} break;
    case 2: return 2;
    default: autodir = 2; break;
    }
  }
  if (col > 0 && fprox[3] == fprox[4] && !wallExists(m[row][col],3)){
    switch (dir) {
    case 1: if(fprox[3] < fprox[0] &&  fprox[3] < fprox[2]){return 3;} break;
    case 3: return 3;
    default: autodir = 3; break;
    }
  }
  return autodir;
} // end autoDirection
