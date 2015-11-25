#define SIZE 8         // Size of the mouse maze.
#define UCEN SIZE/2    // Upper cell value that corresponds to goal cell locations.
#define LCEN UCEN - 1  // Lower cell value that corresponds to goal cell locations.
#define RPM 255        // Motor default run speed.

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor *motorL = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *motorR = AFMS.getStepper(200, 1);

char m[SIZE][SIZE];      // Mouse maze wall values.
char f[SIZE][SIZE];      // Mouse maze flood values.
boolean p[SIZE][SIZE];   // Best path detection array (mouse remembers best path).
boolean pchange;         // If mouse changes its path.
byte row = 0;            // Mouse global row position value.
byte col = 0;            // Mouse global column position value.
byte dir = 0;            // Mouse direction/orientation (NESW).
boolean select = false;  // Selecter switch for diagonal sensors.


void setup() {
  // begin serial communications at 9600 bits of data per second
  // for communicating with the computer, use one of these rates: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200
  Serial.begin(9600);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  pinMode(6, OUTPUT);
  AFMS.begin();
  motorL->setSpeed(RPM);
  motorR->setSpeed(RPM);
  startWalls();
} // end setup


void loop(){
  delay(1000);
  autoPilot(row, col, dir, UCEN, UCEN);
  autoPilot(row, col, dir, 0, 0);
}


void startWalls(){
  //m[0][0] = 14;
  //m[0][SIZE] = 7;
  //m[SIZE][0] = 13;
  //m[SIZE][SIZE] = 11;
  
  m[LCEN][LCEN] = 3;
  m[LCEN][UCEN] = 9;
  m[UCEN][LCEN] = 6;
  m[UCEN][UCEN] = 12;
} // end startWalls


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


void autoPilot(byte &row, byte &col, byte &dir, float targetrow, float targetcol) {
  int autodir, autodirnum;  
  senseWall(m, dir, row, col); // senses the initial cell's walls (initialized now).
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
    if (f[row][col] <= 0) { // if target has been reached or is closed off...
      return;
    }
    
    autodir = -1;
    autodirnum = f[row][col];
    if (row < SIZE-1) {
      // If cell above has a lower flood value and is accessible
      if (f[row+1][col] < autodirnum && !wallExists(m[row][col],0)) {
        autodir = 0;
      }
    }
    if (row > 0) {
      // If cell below ... (same as above)
      if (f[row-1][col] < autodirnum && !wallExists(m[row][col],2)) {
        autodir = 2;
      }
    }
    if (col < SIZE - 1) {
      // If cell to the right ... (same as the first)
      if (f[row][col+1] < autodirnum && !wallExists(m[row][col],1)) {
        autodir = 1;
      }
    }
    if (col > 0) {
      // If cell to the left ... (same as the first)
      if (f[row][col-1] < autodirnum && !wallExists(m[row][col],3)) {
        autodir = 3;
      }
    }
    
    switch (autodir) { // if a valid move has been determined, move there based on its direction
      case 0: moveN(dir, row, col); break;
      case 1: moveE(dir, row, col); break;
      case 2: moveS(dir, row, col); break;
      case 3: moveW(dir, row, col); break;
      default: break;
    }
    
    senseWall(m, dir, row, col);
    
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

/*
void printFullMaze(char a[SIZE][SIZE], byte row, byte col) {
  //----------//Print Full Maze//----------//
  // Prints a succinct version of the full maze
  for (byte i = 0; i < SIZE; i++) {
    Serial.print(" _");
  }
  for (byte i = SIZE - 1; i < 255; i--) {  // byte is unsigned, so -1 is actually 255; still iterates SIZE times
    Serial.print("\n|");
    for (byte j = 0; j < SIZE; j++) {
      if (wallExists(a[i][j],2)) {
        if (i == row && j == col) {
          Serial.print((char)177);
        }
        else {
          Serial.print("_");
        }
      }
      else {
        if (i == row && j == col) {
          Serial.print("+");
        }
        else {
          Serial.print(" ");
        }
      }
      if (wallExists(a[i][j],1)) {
        Serial.print("|");
      }
      else {
        Serial.print(" ");
      }
    }
    Serial.print("|");
  }
  Serial.print("\n");
} // end printFullMaze
*/
