#define SIZE 4          // size of one side of square maze
#define UCEN SIZE/2     // upper center goal value
#define LCEN UCEN - 1   // lowe rcenter goal value

// MAZE STUFF //
byte m[SIZE][SIZE];     // mouse maze wall values
char f[SIZE][SIZE];     // flood values
byte d[SIZE*SIZE];      // directions array for fast run
byte dCount;            // counts number of moves in directions array
boolean p[SIZE][SIZE];  // best path detection array
boolean pchange;        // used to detect if the path has changed
byte row = 0;           // mouse global position values
byte col = 0;
byte dir = 0;
boolean flood = false;  // toggle display of flood values
// end MAZE STUFF //

const byte buttonPin = 2;     // the number of the pushbutton pin
const byte ledPin =  13;      // the number of the LED pin

// MOTOR STUFF //
#include <AccelStepper.h>
AccelStepper motorL(4, 12, 11, 10, 9);
AccelStepper motorR(4, 4, 5, 6, 7);
float spd = 500;
int i = 0;
int counterForward = 0;
int counterTurn = 0;
// end MOTOR STUFF //


void setup() {
  
  motorL.setMaxSpeed(1000.0);  // motor setup
  motorL.setSpeed(spd);
  motorR.setMaxSpeed(1000.0);
  motorR.setSpeed(spd);
  
  // begin serial communications at 9600 bits of data per second
  // for communicating with the computer, use one of these rates: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200
  Serial.begin(9600);
  
  Serial.println("Calibrating sensors...");
  sensorSetup();
  Serial.println("Sensors calibrated.");
  
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  
  row = 0;                               // reset position variables
  col = 0;
  dir = 0;                               // faces SOUTH initially
  floodFill(-1, -1);                     // initial flood-fill of mouse maze
  senseWall(dir, row, col);              // initial wall-sense at start position
  matchCells();                          // make sure all cell walls match up
  printMaze(row, col, dir, flood);  // print advanced maze the mouse sees
  
  do { // repeat going to center and then back to origin until there is no path change
    autoPilot(row, col, dir, flood, UCEN, UCEN, false, false);
    autoPilot(row, col, dir, flood, 0, 0, false, false);
  } while (pchange);
  delay(1000);
  
  autoPilot(row, col, dir, flood, UCEN, UCEN, false, true);  // do fast run without delay
}

boolean floodFill(int row, int col) {
  //----------//Flood//----------//
  //fills all flood array spaces with -1
  for (byte i = 0; i < SIZE; i++) {
    for (byte j = 0; j < SIZE; j++) {
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
    for (byte i = 0; i < SIZE; i++) {
      for (byte j = 0; j < SIZE; j++) {
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

void matchCells() {
  //Matching//
  //makes sure each cell has the same walls as the adjacent cells
  for (byte i = 0; i < SIZE; i++) {
    for (byte j = 0; j < SIZE; j++) {
      if (i > 0) {              // if cell exists SOUTH of current position
        if (wallExists(m[i - 1][j], 0)) {   // if southern cell has north wall, set current cell's south wall
          wallChange(m[i][j], 2, true);
        }
        else if (wallExists(m[i][j], 2)) {  // if current cell has south wall, set southern cell's north wall
          wallChange(m[i - 1][j], 0, true);
        }
      }
      if (i < SIZE - 1) {       // if cell exists NORTH of current position
        if (wallExists(m[i + 1][j], 2)) {   // if northern cell has south wall, set current cell's north wall
          wallChange(m[i][j], 0, true);
        }
        else if (wallExists(m[i][j], 0)) {  // if current cell has north wall, set northern cell's south wall
          wallChange(m[i + 1][j], 2, true);
        }
      }
      if (j > 0) {              // if cell exists WEST of current position
        if (wallExists(m[i][j - 1], 1)) {   // if western cell has east wall, set current cell's west wall
          wallChange(m[i][j], 3, true);
        }
        else if (wallExists(m[i][j], 3)) {  // if current cell has west wall, set western cell's east wall
          wallChange(m[i][j - 1], 1, true);
        }
      }
      if (j < SIZE - 1) {       // if cell exists EAST of current position
        if (wallExists(m[i][j + 1], 3)) {   // if eastern cell has west wall, set current cell's east wall
          wallChange(m[i][j], 1, true);
        }
        else if (wallExists(m[i][j], 1)) {  // if current cell has east wall, set eastern cell's west wall
          wallChange(m[i][j + 1], 3, true);
        }
      }
    }
  }
} // end matchCells

void clearWalls() {
  //Clean Slate//
  //starts the maze without having any walls anywhere (clears flood values as well)
  for (byte i = 0; i < SIZE; i++) {
    for (byte j = 0; j < SIZE; j++) {
      m[i][j] = 0;
      f[i][j] = 0;
    }
  }
} // end clearWalls

void printMaze(byte row, byte col, byte dir, boolean flood) {
  //----------//Print Mouse Maze//----------//
  // Prints the maze that the mouse sees
  // Prints the walls of the maze
  // Also prints the mouse and its orientation, as well as the flood values if toggled on
  char wall[16] = {(char)32, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35, 
                   (char)35, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35};
  byte n = 0;
  
  for (byte i = SIZE - 1; i < 255; i--) { // for a byte, -1 = 255, so this will still iterate SIZE times
    
    for (byte j = 0; j < SIZE; j++) { // places ceiling of each row
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
    
    for (byte j = 0; j < SIZE; j++) { // places walls and spaces of each row
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
      for (byte j = 0; j < SIZE; j++) { // places floor of maze
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

void autoPilot(byte &row, byte &col, byte &dir, bool flood, float targetrow, float targetcol, boolean delayOn, boolean finalRun) {
  byte autodir, autodirnum;  // stores next direction value and lowest floodfill value of surrounding cells
  byte startDir = dir;  // used for the final run
  
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
      floodFill((int)targetrow, (int)targetcol);
    }

    if (f[row][col] <= 0) { // if target has been reached (0) or is closed off (-1)
      if (finalRun) {  // if this was the last run, reset position to that of the bot and execute stored moves
        row = 0;
        col = 0;
        dir = startDir;
        for (byte i = 0; i < dCount; i++) {
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
      printMaze(row, col, dir, flood);
      return;  // stops autopilot
    }

    autodirnum = f[row][col];  // initialize lowest floodfill value of cells surrounding current cell
    if (row < SIZE - 1) {
      // If northern cell has a lower flood value and is accessible, set next direction as NORTH
      if (f[row + 1][col] < autodirnum && !wallExists(m[row][col], 0)) {
        autodir = 0;
      }
    }
    if (row > 0) {
      // If southern cell has a lower flood value and is accessible, set next direction as SOUTH
      if (f[row - 1][col] < autodirnum && !wallExists(m[row][col], 2)) {
        autodir = 2;
      }
    }
    if (col < SIZE - 1) {
      // If eastern cell has a lower flood value and is accessible, set next direction as EAST
      if (f[row][col + 1] < autodirnum && !wallExists(m[row][col], 1)) {
        autodir = 1;
      }
    }
    if (col > 0) {
      // If western cell has a lower flood value and is accessible, set next direction as WEST
      if (f[row][col - 1] < autodirnum && !wallExists(m[row][col], 3)) {
        autodir = 3;
      }
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
    printMaze(row, col, dir, flood);

  }
} // end autoPilot

void loop() {
  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
//  if (digitalRead(buttonPin) == HIGH) {
//    while (digitalRead(buttonPin) == HIGH) { // delay until button is depressed
//      delay(100);
//    }
//    Serial.println("Button pressed");
//    // DO SOMETHING HERE
//  }
  
  delay(1000);  // putting to sleep to reduce power loss
  
} // end loop
