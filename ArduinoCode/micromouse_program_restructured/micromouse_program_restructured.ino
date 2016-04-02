#define SIZE 4          // size of one side of square maze
#define UCEN SIZE/2     // upper center goal value
#define LCEN UCEN - 1   // lower center goal value

// MAZE VARIABLES //
unsigned char m[SIZE][SIZE];     // mouse maze wall values
char f[SIZE][SIZE];     // flood values
unsigned char d[SIZE*SIZE];      // directions array for fast run
unsigned char dCount;            // counts number of moves in directions array
boolean p[SIZE][SIZE];  // best path detection array
boolean pchange;        // used to detect if the path has changed
unsigned char row = 0;           // mouse global position values
unsigned char col = 0;
unsigned char dir = 0;
boolean flood = false;  // toggle display of flood values
// end MAZE VARIABLES //

// MOTOR VARIABLES //
#include <AccelStepper.h>
AccelStepper motorL(4, 12, 11, 10, 9);
AccelStepper motorR(4, 4, 5, 6, 7);
float spd = 500;
//int i = 0;  //unsure if needed
char counterForward = 0;
char counterTurn = 0;
// end MOTOR VARIABLES //

//setup
//
//sets up mouse
//runs autoPilot for initial, optimization, and final run
void setup() {

//******** Setup ************
  // motor 
  motorL.setMaxSpeed(1000.0);  
  motorL.setSpeed(spd);
  motorR.setMaxSpeed(1000.0);
  motorR.setSpeed(spd);
  
  // serial 
  Serial.begin(9600);

  //sensors
  Serial.println("Calibrating sensors...");
  sensorSetup();
  Serial.println("Sensors calibrated.");
  
  row = 0;                               // reset position variables
  col = 0;
  dir = 0;                               // faces SOUTH initially
  floodFill(-1, -1);                     // initial flood-fill of mouse maze
  senseWall(dir, row, col);              // initial wall-sense at start position
  matchCells();                          // make sure all cell walls match up
  printMaze(row, col, dir);       // print advanced maze the mouse sees

  // finds center and then optimizes path
  do { 
    autoPilot(row, col, dir, UCEN, UCEN, false, false);
    autoPilot(row, col, dir, 0, 0, false, false);
  } while (pchange);
  delay(1000);

 //Fast run
  autoPilot(row, col, dir, UCEN, UCEN, false, true);  
}//end setup()

//floodFill
//
//uses row, col position as goal
void floodFill(char row, char col) {
  //fills all flood array spaces with -1
  for (unsigned char i = 0; i < SIZE; i++) {
    for (unsigned char j = 0; j < SIZE; j++) {
      f[i][j] = -1;
    }
  }

  //fills the goal flood array spaces with 0 by default
  if (row < 0 || col < 0) { // if row or col are negative, set center squares as goals
    f[LCEN][LCEN] = 0;
    f[LCEN][UCEN] = 0;
    f[UCEN][LCEN] = 0;
    f[UCEN][UCEN] = 0;
  }
  else {                    // else, set input row, col square as goal
    f[row][col] = 0;
  }

  // fills the flood array with values using flood fill logic
  for (int k = 0; k < SIZE*SIZE; k++) {  // byte's range is 0-255, so int type will stop infinite loop from being created when SIZE = 16
    for (unsigned char i = 0; i < SIZE; i++) {
      for (unsigned char j = 0; j < SIZE; j++) {
        if (f[i][j] == k) {  // if the flood array space equals k (starting at 0), place k + 1 in adjacent flood array spaces
          if (i < SIZE - 1) {  // if cell exists north of current position
            if (!wallExists(m[i + 1][j], 2) && f[i + 1][j] == -1) {  // North
              f[i + 1][j] = f[i][j] + 1;
            }
          }
          if (i > 0) {  // if cell exists south of current position
            if (!wallExists(m[i - 1][j], 0) && f[i - 1][j] == -1) {  // South
              f[i - 1][j] = f[i][j] + 1;
            }
          }
          if (j < SIZE - 1) {  // if cell exists east of current position
            if (!wallExists(m[i][j + 1], 3) && f[i][j + 1] == -1) {  // East
              f[i][j + 1] = f[i][j] + 1;
            }
          }
          if (j > 0) {  // if cell exists west of current position
            if (!wallExists(m[i][j - 1], 1) && f[i][j - 1] == -1) {  // West
              f[i][j - 1] = f[i][j] + 1;
            }
          }
        }
      }
    }
  }
} // end floodFill

void printMaze(unsigned char row, unsigned char col, unsigned char dir) {
  //----------//Print Mouse Maze//----------//
  // Prints the maze that the mouse sees
  // Prints the walls of the maze
  // Also prints the mouse and its orientation, as well as the flood values if toggled on
  char wall[16] = {(char)32, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35, 
                   (char)35, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35};
  unsigned char n = 0;
  
  for (unsigned char i = SIZE - 1; i < 255; i--) { // for a byte, -1 = 255, so this will still iterate SIZE times
    
    for (unsigned char j = 0; j < SIZE; j++) { // places ceiling of each row
      n = 0;
      if (i < SIZE - 1) { // if not the top row, adds the north wall for each peg
        n = wallExists(m[i + 1][j], 3);
      }
      if (j > 0) { // if not the first column, adds the west wall for each peg
        n = n + wallExists(m[i][j - 1], 0);
      }
      n = n + wallExists(m[i][j], 0) + wallExists(m[i][j], 3); // sets n to be the number the corner represents
      Serial.print(wall[n]);
      Serial.print(wall[wallExists(m[i][j], 0)]);
      Serial.print(wall[wallExists(m[i][j], 0)]);
      if (j == SIZE - 1) { // places far left wall
        n = 0;
        if (i < SIZE - 1) { // if not the top row, adds north wall
          n = wallExists(m[i + 1][j], 1);
        }
        n = n + wallExists(m[i][j], 1) + wallExists(m[i][j], 0);
        Serial.print(wall[n]);
      }
    }
    Serial.print("\n");
    
    for (unsigned char j = 0; j < SIZE; j++) { // places walls and spaces of each row
      Serial.print(wall[wallExists(m[i][j], 3)]); // prints wall if it exists
      if (i == row && j == col) { // if mouse is in the space, prints according to orientation
        switch (dir) {
          case 0: Serial.print("/\\");  break;
          case 1: Serial.print(">>");  break;
          case 2: Serial.print("\\/");  break;
          case 3: Serial.print("<<");  break;
          default: Serial.print("XX");  break;
        }
      }
      else if (flood) { // if flood is toggled on, prints the flood value
        if (f[i][j] < 0 || f[i][j] > 9) { // if value takes up two spaces, don't print a space before
          Serial.print(f[i][j], DEC);
        }
        else {
          Serial.print(" ");
          Serial.print(f[i][j], DEC);
        }
      }
      else { // else, prints a space
        Serial.print("  ");
      }
      if (j == SIZE - 1) {
        Serial.print(wall[wallExists(m[i][j], 1)]);
      }
    }
    Serial.print("\n");
    
    if (i == 0) { // if last row, prints the floor as well
      for (unsigned char j = 0; j < SIZE; j++) { // places floor of maze
        n = 0;
        if (j > 0) {
          n = n + wallExists(m[i][j - 1], 2);
        }
        n = n + wallExists(m[i][j], 3) + wallExists(m[i][j], 2);
        Serial.print(wall[n]);
        Serial.print(wall[wallExists(m[i][j], 2)]);
        Serial.print(wall[wallExists(m[i][j], 2)]);
        if (j == SIZE - 1) { // if last column, prints bottom right corner
          Serial.print(wall[wallExists(m[i][j], 1) + wallExists(m[i][j], 2)]);
        }
      }
      Serial.print("\n");
    }
    
  }
} // end printMaze

void autoPilot(unsigned char &row, unsigned char &col, unsigned char &dir, unsigned char targetrow, unsigned char targetcol, boolean delayOn, boolean finalRun) {
  unsigned char autodir = 10; 
  unsigned char autodirnum;  // stores next direction value and lowest floodfill value of surrounding cells
  unsigned char startDir = dir;  // used for the final run
  
  senseWall(dir, row, col); // senses the initial cell's walls
  matchCells();
  
  p[row][col] = true;  // cell currently standing in is part of the path
  pchange = false;  // initially no change in path
  
  while (true) {

    // Floodfills mouse's maze array from different locations depending on the target
    if ((targetrow == UCEN || targetrow == LCEN) && (targetcol == UCEN || targetcol == LCEN)) {  // if target is somewhere in center, ...
      floodFill(-1, -1);                                                                         // floodfill with center as goal
    }
    else {  // if target is where else, floodfill from target cell
      floodFill((char)targetrow, (char)targetcol);
    }

    if (f[row][col] <= 0) { // if target has been reached (0) or is closed off (-1)
      if (finalRun) {  // if this was the last run, reset position to that of the bot and execute stored moves
        row = 0;
        col = 0;
        dir = startDir;
        for (unsigned char i = 0; i < dCount; i++) {
          switch (d[i]) {  // execute buffered moves from directions array
            case 0: moveN(dir, row, col, false); break;
            case 1: moveE(dir, row, col, false); break;
            case 2: moveS(dir, row, col, false); break;
            case 3: moveW(dir, row, col, false); break;
            default: break;
          }
        }
      }
      if ((targetrow == UCEN || targetrow == LCEN) && (targetcol == UCEN || targetcol == LCEN)) {  // if target is somewhere in center, ...
        m[LCEN][LCEN] = wallSet(false, false, true, true);                                         // set center walls
        m[LCEN][UCEN] = wallSet(false, true, true, false);
        m[UCEN][UCEN] = wallSet(true, true, false, false);
        m[UCEN][LCEN] = wallSet(true, false, false, true);
        
        wallChange(m[row][col], (dir + 2)%4, false);  // wall behind mouse is open
      }
      // Prints the maze to the screen when it reaches its destination
      matchCells();
      printMaze(row, col, dir);
      return;  // stops autopilot
    }

    autodirnum = f[row][col];  // initialize lowest floodfill value of cells surrounding current cell
    if (row < SIZE - 1 && (f[row + 1][col] < autodirnum && !wallExists(m[row][col], 0))) {
      // If northern cell has a lower flood value and is accessible, set next direction as NORTH
      autodir = 0;
    }
    else if (row > 0 &&(f[row - 1][col] < autodirnum && !wallExists(m[row][col], 2))) {
      // If southern cell has a lower flood value and is accessible, set next direction as SOUTH
      autodir = 2;
    }
    else if (col < SIZE - 1 && (f[row][col + 1] < autodirnum && !wallExists(m[row][col], 1))) {
      // If eastern cell has a lower flood value and is accessible, set next direction as EAST
      autodir = 1;
    }
    else if (col > 0 && (f[row][col - 1] < autodirnum && !wallExists(m[row][col], 3))) {
      // If western cell has a lower flood value and is accessible, set next direction as WEST
      autodir = 3;
    }
    switch (autodir) { // if a valid move has been determined, move there based on its direction
      case 0: moveN(dir, row, col, finalRun); break;
      case 1: moveE(dir, row, col, finalRun); break;
      case 2: moveS(dir, row, col, finalRun); break;
      case 3: moveW(dir, row, col, finalRun); break;
      default: break;
    }
    if (finalRun) {  // if on the final run, store the directions into the directions array
      d[dCount++] = autodir;
    }
    
    if (delayOn) {  // adds delay if specified in function call
      delay(1000);
    }
    
    if (p[row][col] == false) { // if never been on this cell before, indicate path change and add it to path
      pchange = true;
      p[row][col] = true;
      
      senseWall(dir, row, col);  // sense current cell's walls on new cells only
      matchCells();
    }
    
    // Prints the maze to the screen
    printMaze(row, col, dir);

  }
} // end autoPilot

void loop() {
  
  delay(100000);  // putting to sleep to reduce power loss
  
} // end loop
