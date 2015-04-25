#define SIZE 4          // size of one side of square maze
#define UCEN SIZE/2     // upper center goal value
#define LCEN UCEN - 1   // lowe rcenter goal value

char m[SIZE][SIZE][5];  // mouse maze and flood values
boolean p[SIZE][SIZE];  // best path detection array
boolean pchange;        // used to detect if the path has changed
byte row = 0;           // mouse global position values
byte col = 0;
byte dir = 0;
boolean flood = false;  // toggle display of flood values

const byte buttonPin = 2;     // the number of the pushbutton pin
const byte ledPin =  13;      // the number of the LED pin

void setup() {
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
  
  row = 0;                                  // reset position variables
  col = 0;
  dir = 2;                                  // faces SOUTH initially
  floodFill(m, -1, -1);                     // initial flood-fill of mouse maze
  senseWall(dir, row, col, m);              // initial wall-sense at start position (*reasoning*)
  matchCells(m);                            // make sure all cell walls match up
  printMouseMaze(m, row, col, dir, flood);  // print advanced maze the mouse sees
}

boolean floodFill(char b[SIZE][SIZE][5], int row, int col) {
  //----------//Flood//----------//
  //fills all flood array spaces with -1
  for(byte i = 0; i < SIZE; i++) {
    for(byte j = 0; j < SIZE; j++) {
      b[i][j][4] = -1;
    }
  }

  //fills the goal flood array spaces with 0 by default
  if (row < 0 || col < 0) { // if row or col are negative, set center squares as goals
    b[LCEN][LCEN][4] = 0;
    b[LCEN][UCEN][4] = 0;
    b[UCEN][LCEN][4] = 0;
    b[UCEN][UCEN][4] = 0;
  }
  else {                    // else, set input row, col square as goal
    b[row][col][4] = 0;
  }

  // fills the flood array with values using flood fill logic
  for (int k = 0; k < SIZE*SIZE; k++) {  // byte's range is 0-255, so int type will stop infinite loop from being created when SIZE = 16
    for (byte i = 0; i < SIZE; i++) {
      for (byte j = 0; j < SIZE; j++) {
        if (b[i][j][4] == k) {  // if the flood array space equals k (starting at 0), place k + 1 in adjacent flood array spaces
          if (i < SIZE - 1) {  // if cell exists north of current position
            if (!b[i + 1][j][2] && b[i + 1][j][4] == -1) {  // North
              b[i + 1][j][4] = b[i][j][4] + 1;
            }
          }
          if (i > 0) {  // if cell exists south of current position
            if (!b[i - 1][j][0] && b[i - 1][j][4] == -1) {  // South
              b[i - 1][j][4] = b[i][j][4] + 1;
            }
          }
          if (j < SIZE - 1) {  // if cell exists east of current position
            if (!b[i][j + 1][3] && b[i][j + 1][4] == -1) {  // East
              b[i][j + 1][4] = b[i][j][4] + 1;
            }
          }
          if (j > 0) {  // if cell exists west of current position
            if (!b[i][j - 1][1] && b[i][j - 1][4] == -1) {  // West
              b[i][j - 1][4] = b[i][j][4] + 1;
            }
          }
        }
      }
    }
  }
  
  // checks if all four corners can reach the center
  if (b[0][0][4] != -1 && b[0][SIZE - 1][4] != -1 && b[SIZE - 1][0][4] != -1 && b[SIZE - 1][SIZE - 1][4] != -1) {
    return true;
  }
  return false;  // returns false if not solvable
} // end floodFill

void matchCells(char b[SIZE][SIZE][5]) {
  //Matching//
  //makes sure each cell has the same walls as the adjacent cells
  for (byte i = 0; i < SIZE; i++) {
    for (byte j = 0; j < SIZE; j++) {
      if (i > 0) {              // if cell exists SOUTH of current position
        if (b[i - 1][j][0]) {   // if southern cell has north wall, set current cell's south wall
          b[i][j][2] = 1;
        }
        else if (b[i][j][2]) {  // if current cell has south wall, set southern cell's north wall
          b[i - 1][j][0] = 1;
        }
      }
      if (i < SIZE - 1) {       // if cell exists NORTH of current position
        if (b[i + 1][j][2]) {   // if northern cell has south wall, set current cell's north wall
          b[i][j][0] = 1;
        }
        else if (b[i][j][0]) {  // if current cell has north wall, set northern cell's south wall
          b[i + 1][j][2] = 1;
        }
      }
      if (j > 0) {              // if cell exists WEST of current position
        if (b[i][j - 1][1]) {   // if western cell has east wall, set current cell's west wall
          b[i][j][3] = 1;
        }
        else if (b[i][j][3]) {  // if current cell has west wall, set western cell's east wall
          b[i][j - 1][1] = 1;
        }
      }
      if (j < SIZE - 1) {       // if cell exists EAST of current position
        if (b[i][j + 1][3]) {   // if eastern cell has west wall, set current cell's east wall
          b[i][j][1] = 1;
        }
        else if (b[i][j][1]) {  // if current cell has east wall, set eastern cell's west wall
          b[i][j + 1][3] = 1;
        }
      }
    }
  }
} // end matchCells

void clearWalls(char b[SIZE][SIZE][5]) {
  //Clean Slate//
  //starts the maze without having any walls anywhere (clears flood values as well)
  for (byte i = 0; i < SIZE; i++) {
    for (byte j = 0; j < SIZE; j++) {
      for (byte k = 0; k < 5; k++) {
        b[i][j][k] = 0;
      }
    }
  }
} // end clearWalls

void printMouseMaze(char b[SIZE][SIZE][5], byte row, byte col, byte dir, boolean flood) {
  //----------//Print Mouse Maze//----------//
  // Prints the maze that the mouse sees
  // Prints the walls of the maze
  // Also prints the mouse and its orientation, as well as the flood values if toggled on
  char wall[16] = {(char)32, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35, 
                   (char)32, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35};
  byte n = 0;
  
  for (byte i = SIZE - 1; i < 255; i--) { // for a byte, -1 = 255, so this will still iterate 16 times
    
    for (byte j = 0; j < SIZE; j++) { // places ceiling of each row
      n = 0;
      if (i < SIZE - 1) { // if not the top row, adds the north wall for each peg
        n = b[i + 1][j][3];
      }
      if (j > 0) { // if not the first column, adds the west wall for each peg
        n = n + b[i][j - 1][0];
      }
      n = n + b[i][j][0] + b[i][j][3]; // sets n to be the number the corner represents
      Serial.print(wall[n]);
      Serial.print(wall[b[i][j][0]]);
      Serial.print(wall[b[i][j][0]]);
      if (j == SIZE - 1) { // places far left wall
        n = 0;
        if (i < SIZE - 1) { // if not the top row, adds north wall
          n = b[i + 1][j][1];
        }
        n = n + b[i][j][1] + b[i][j][0];
        Serial.print(wall[n]);
      }
    }
    Serial.print("\n");
    
    for (byte j = 0; j < SIZE; j++) { // places walls and spaces of each row
      Serial.print(wall[b[i][j][3]]); // prints wall if it exists
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
        if (b[i][j][4] < 0 || b[i][j][4] > 9) { // if value takes up two spaces, don't print a space before
          Serial.print(b[i][j][4], DEC);
        }
        else {
          Serial.print(" ");
          Serial.print(b[i][j][4], DEC);
        }
      }
      else { // else, prints a space
        Serial.print("  ");
      }
      if (j == SIZE - 1) {
        Serial.print(wall[b[i][j][1]]);
      }
    }
    Serial.print("\n");
    
    if (i == 0) { // if last row, prints the floor as well
      for (byte j = 0; j < SIZE; j++) { // places floor of maze
        n = 0;
        if (j > 0) {
          n = n + b[i][j - 1][2];
        }
        n = n + b[i][j][3] + b[i][j][2];
        Serial.print(wall[n]);
        Serial.print(wall[b[i][j][2]]);
        Serial.print(wall[b[i][j][2]]);
        if (j == SIZE - 1) { // if last column, prints bottom right corner
          Serial.print(wall[b[i][j][1] + b[i][j][2]]);
        }
      }
      Serial.print("\n");
    }
    
  }
} // end printMouseMaze

void autoPilot(char c[SIZE][SIZE][5], byte &row, byte &col, byte &dir, bool flood, float targetrow, float targetcol) { // c is mouse maze
  byte autodir, autodirnum;  // stores next direction value and lowest floodfill value of surrounding cells
  
  senseWall(dir, row, col, c); // senses the initial cell's walls
  matchCells(c);
  
  p[row][col] = true;  // cell currently standing in is part of the path
  pchange = false;  // initially no change in path
  
  while (true) {

    // Floodfills mouse's maze array from different locations depending on the target
    if ((targetrow == UCEN || targetrow == LCEN) && (targetcol == UCEN || targetcol == LCEN)) {  // if target is somewhere in center, ...
      floodFill(c, -1, -1);                                                                      // floodfill with center as goal
    }
    else {  // if target is where else, floodfill from target cell
      floodFill(c, (int)targetrow, (int)targetcol);
    }

    if (c[row][col][4] <= 0) { // if target has been reached (0) or is closed off (-1)
//      for (int i = LCEN; i <= UCEN; i++) {
//        for (int j = LCEN; j <= UCEN; j++) {
//          for (int k = 0; k < 4; k++) {
//            c[i][j][k] = b[i][j][k]; // set walls of center equal to actual maze walls
//          }
//        }
//      }
      // Prints the maze(s) to the screen
      matchCells(c);
//      printFullMaze(b, row, col);
      printMouseMaze(c, row, col, dir, flood);
      return;  // stops autopilot
    }

    autodir = -1;  // initial autopilot direction
    autodirnum = c[row][col][4];  // initialize lowest floodfill value of cells surrounding current cell
    if (row < SIZE - 1) {
      // If northern cell has a lower flood value and is accessible, set next direction as NORTH
      if (c[row + 1][col][4] < autodirnum && !c[row][col][0]) {
        autodir = 0;
      }
    }
    if (row > 0) {
      // If southern cell has a lower flood value and is accessible, set next direction as SOUTH
      if (c[row - 1][col][4] < autodirnum && !c[row][col][2]) {
        autodir = 2;
      }
    }
    if (col < SIZE - 1) {
      // If eastern cell has a lower flood value and is accessible, set next direction as EAST
      if (c[row][col + 1][4] < autodirnum && !c[row][col][1]) {
        autodir = 1;
      }
    }
    if (col > 0) {
      // If western cell has a lower flood value and is accessible, set next direction as WEST
      if (c[row][col - 1][4] < autodirnum && !c[row][col][3]) {
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

    senseWall(dir, row, col, c);  // sense current cell's walls
    matchCells(c);
    
    if (p[row][col] == false) { // if never been on this cell before, indicate path change and add it to path
      pchange = true;
      p[row][col] = true;
    }
    
    // Prints the maze(s) to the screen
//    printFullMaze(b, row, col);
//    printMouseMaze(c, row, col, dir, flood);
//    Serial.print("Going to: ");
//    Serial.print(targetrow);
//    Serial.print(", ");
//    Serial.println(targetcol);
    //Sleep(250);

  }
} // end autoPilot

const int length = 6;  // predefined input array length
int input[length] = { '\0' };
int clock = (int)millis();  // timing clock

// shifts array up and puts 'next' in first position (0 by default)
void arrayShift(int my_arr[], int next = 0) {
  for(int i = length - 1; i > 0; i--) {
    my_arr[i] = my_arr[i - 1];
  }
  my_arr[0] = next;
} // end arrayShift

void loop() {
  
  // send data only when data is received:
  if (Serial.available() > 0) {
    // read the incoming byte and put in "buffer"
    arrayShift(input, Serial.read());
    
    if (input[0] >= 65 && input[0] <= 90) {  // uppercase to lowercase
      input[0] += 32;
    }
    
    // "clear" input
    if (input[4] == 'c' && input[3] == 'l' && input[2] == 'e' && input[1] == 'a' && input[0] == 'r') {
      
      clearWalls(m);                            // clear mouse's maze
      
      for (byte i = 0; i < SIZE; i++) {         // clear path array
        for (byte j = 0; j < SIZE; j++) {
            p[i][j] = false;
        }
      }
      
      row = 0;                                  // reset mouse position variables
      col = 0;
      dir = 2;                                  // facing SOUTH intially
      floodFill(m, -1, -1);                     // initial flood-fill of mouse maze
      
      Serial.println("Cleared.");
      
    }
    
    // "f" input
    if (input[0] == 'f') {  // toggles display of flood values and reprints maze
      flood  = !flood;
      printMouseMaze(m, row, col, dir, flood);
    }
    
    // "solve" input
    if (input[4] == 's' && input[3] == 'o' && input[2] == 'l' && input[1] == 'v' && input[0] == 'e') {
      digitalWrite(ledPin, LOW);
      Serial.println("Sure thing.  I'll begin solving right away.  Thanks for asking so politely!");
      
      do { // repeat going to center and then back to origin until there is no path change
        autoPilot(m, row, col, dir, flood, UCEN, UCEN);
        autoPilot(m, row, col, dir, flood, 0, 0);
      } while (pchange);
//      digitalWrite(ledPin, HIGH);
      
    }
    
    // "center" input
    if (input[5] == 'c' && input[4] == 'e' && input[3] == 'n' && input[2] == 't' && input[1] == 'e' && input[0] == 'r') {
      Serial.println("Going to center.");
      autoPilot(m, row, col, dir, flood, UCEN, UCEN);
    }
    
    // "return" input
    if (input[5] == 'r' && input[4] == 'e' && input[3] == 't' && input[2] == 'u' && input[1] == 'r' && input[0] == 'n') {
      Serial.println("Going to origin.");
      autoPilot(m, row, col, dir, flood, 0, 0);
    }
    
    clock = (int)millis();  // reset clock
  }
  
  // if timer expires, reset input "buffer" array
  if ((int)millis() - clock >= 100) {  // resets after 100 ms
    for (byte i = 0; i < length; i++) {
      input[i] = 0;
    }
    delay(100);
    clock = (int)millis();
  }
  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
//  if (digitalRead(buttonPin) == HIGH) {
//    while (digitalRead(buttonPin) == HIGH) { // delay until button is depressed
//      delay(100);
//    }
//    Serial.println("Going to center.");
//    autoPilot(g, m, row, col, dir, flood, UCEN, UCEN);
//  }
  
} // end loop
