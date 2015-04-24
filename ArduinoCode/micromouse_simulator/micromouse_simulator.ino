#define SIZE 8
#define UCEN SIZE/2
#define LCEN UCEN - 1

char g[SIZE][SIZE][5];  // generated maze and flood values
char m[SIZE][SIZE][5];  // mouse maze and flood values
boolean p[SIZE][SIZE];  // best path detection array
boolean pchange;
byte row = 0;  // mouse global position values
byte col = 0;
byte dir = 0;
boolean flood = false;

const byte buttonPin = 2;     // the number of the pushbutton pin
const byte ledPin =  13;      // the number of the LED pin

void setup() {
  // begin serial communications at 9600 bits of data per second
  // for communicating with the computer, use one of these rates: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200
  Serial.begin(9600);
  
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  
  // generate maze at startup
  Serial.println("Generating...");
  randomMaze(g);
  Serial.println("Generated");
  Serial.println("Printing...");
  row = 0;
  col = 0;
  dir = 0;
  floodFill(g, -1, -1);
  senseWall(dir, row, col, g, m);
  matchCells(m);
  printFullMaze(g, row, col);
  printMouseMaze(m, row, col, dir, flood);
  Serial.println("Printed");
}

void printFullMaze(char b[SIZE][SIZE][5], byte row, byte col) {
  //----------//Print Full Maze//----------//
  // Prints a succinct version of the full maze
  for (byte i = 0; i < SIZE; i++) {
    Serial.print(" _");
  }
  for (byte i = SIZE - 1; i < 255; i--) {  // byte is unsigned, so -1 is actually 255; still iterates SIZE times
    Serial.print("\n|");
    for (byte j = 0; j < SIZE; j++) {
      if (b[i][j][2]) {
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
      if (b[i][j][1]) {
        Serial.print("|");
      }
      else {
        Serial.print(" ");
      }
    }
  }
  Serial.print("\n");
} // end printFullMaze

boolean floodFill(char b[SIZE][SIZE][5], int row, int col) {
  //----------//Flood//----------//
  //fills all flood array spaces with -1
  for(byte i = 0; i < SIZE; i++) {
    for(byte j = 0; j < SIZE; j++) {
      b[i][j][4] = -1;
    }
  }

  //fills the four goal flood array spaces with 0 by default
  if (row < 0 || col < 0) {
    b[LCEN][LCEN][4] = 0;
    b[LCEN][UCEN][4] = 0;
    b[UCEN][LCEN][4] = 0;
    b[UCEN][UCEN][4] = 0;
  }
  else {
    b[row][col][4] = 0;
  }

  //fills the flood array with values using flood fill logic
  for (int k = 0; k < SIZE*SIZE; k++) {  //byte's range is 0-255, so int will stop infinite loop from being created
    for (byte i = 0; i < SIZE; i++) {
      for (byte j = 0; j < SIZE; j++) {
        if (b[i][j][4] == k) {  //if the flood array space equals k (starting at 0), place k+1 in adjacent flood array spaces
          if (i < SIZE - 1) {
            if (!b[i + 1][j][2] && b[i + 1][j][4] == -1) {  //North
              b[i + 1][j][4] = b[i][j][4] + 1;
            }
          }
          if (i > 0) {
            if (!b[i - 1][j][0] && b[i - 1][j][4] == -1) {  //South
              b[i - 1][j][4] = b[i][j][4] + 1;
            }
          }
          if (j < SIZE - 1) {
            if (!b[i][j + 1][3] && b[i][j + 1][4] == -1) {  //East
              b[i][j + 1][4] = b[i][j][4] + 1;
            }
          }
          if (j > 0) {
            if(!b[i][j - 1][1] && b[i][j - 1][4] == -1) {  //West
              b[i][j - 1][4] = b[i][j][4] + 1;
            }
          }
        }
      }
    }
  }
  
  //checks if all four corners can reach the center
  if (b[0][0][4] != -1 && b[0][SIZE - 1][4] != -1 && b[SIZE - 1][0][4] != -1 && b[SIZE - 1][SIZE - 1][4] != -1) {
    return 1;
  }
  return 0;
} // end floodFill

void matchCells(char b[SIZE][SIZE][5]) {
  //Matching//
  //makes sure each cell has the same walls as the adjacent cells
  for (byte i = 0; i < SIZE; i++) {
    for (byte j = 0; j < SIZE; j++) {
      if (i > 0) {
        if (b[i - 1][j][0]) {
          b[i][j][2] = 1;
        }
        else if (b[i][j][2]) {
          b[i - 1][j][0] = 1;
        }
      }
      if (i < SIZE - 1) {
        if (b[i + 1][j][2]) {
          b[i][j][0] = 1;
        }
        else if (b[i][j][0]) {
          b[i + 1][j][2] = 1;
        }
      }
      if (j > 0) {
        if (b[i][j - 1][1]) {
          b[i][j][3] = 1;
        }
        else if (b[i][j][3]) {
          b[i][j - 1][1] = 1;
        }
      }
      if (j < SIZE - 1) {
        if (b[i][j + 1][3]) {
          b[i][j][1] = 1;
        }
        else if (b[i][j][1]) {
          b[i][j + 1][3] = 1;
        }
      }
    }
  }
} // end matchCells

void generateRandomMaze(char b[SIZE][SIZE][5]) {
  byte percent = 30; // percent chance of placing a wall
  //----------//Walls//----------//
  //build a random maze
  for (byte i = 0; i < SIZE; i++) {
    for (byte j = 0; j < SIZE; j++) {
      //randomize the bottom and left sides of each cell
      for (byte k = 2; k < 4; k++) {
        b[i][j][k] = (char)(random()%100 < percent);
      }
    }
  }

  //----------//Constants//----------//
  //After the maze is randomly generated, these constants are placed
  
  //Center Walls//
  //makes the center squares have walls on all sides
  for (byte i = LCEN; i <= UCEN; i++) {
    for (byte j = LCEN; j <= UCEN; j++) {
      for (byte k = 0; k < 4; k++) {
        b[i][j][k] = 1;
      }
    }
  }

  //Center Opening//
  //places center wall opening
  switch (random()%8) {
    case 0: b[LCEN][LCEN][3] = 0; break;
    case 1: b[LCEN][LCEN][2] = 0; break;
    case 2: b[LCEN][UCEN][2] = 0; break;
    case 3: b[LCEN][UCEN][1] = 0; break;
    case 4: b[UCEN][UCEN][1] = 0; break;
    case 5: b[UCEN][UCEN][0] = 0; break;
    case 6: b[UCEN][LCEN][0] = 0; break;
    case 7: b[UCEN][LCEN][3] = 0; break;
  }

  //Border//
  //make the edges have walls
  for (byte i = 0; i < SIZE; i++) {
    b[0][i][2] = 1;
    b[SIZE - 1][i][0] = 1;
    b[i][0][3] = 1;
    b[i][SIZE - 1][1] = 1;
  }
  
  //Corners//
  //places the starting positions in the four corners
  b[0][0][0] = 0;
  b[0][0][1] = 1;
  b[0][0][2] = 1;
  b[0][0][3] = 1;
  b[0][1][3] = 1;
  b[1][0][2] = 0;
  b[0][SIZE - 1][0] = 1;
  b[0][SIZE - 1][1] = 1;
  b[0][SIZE - 1][2] = 1;
  b[0][SIZE - 1][3] = 0;
  b[0][SIZE - 2][1] = 0;
  b[1][SIZE - 1][2] = 1;
  b[SIZE - 1][0][0] = 1;
  b[SIZE - 1][0][1] = 0;
  b[SIZE - 1][0][2] = 1;
  b[SIZE - 1][0][3] = 1;
  b[SIZE - 1][1][3] = 0;
  b[SIZE - 2][0][0] = 1;
  b[SIZE - 1][SIZE - 1][0] = 1;
  b[SIZE - 1][SIZE - 1][1] = 1;
  b[SIZE - 1][SIZE - 1][2] = 0;
  b[SIZE - 1][SIZE - 1][3] = 1;
  b[SIZE - 1][SIZE - 2][1] = 1;
  b[SIZE - 2][SIZE - 1][0] = 0;

  matchCells(b);

  //Pegs//
  //makes sure a wall is on every peg
  for (byte i = 0; i < SIZE; i++) {
    for (byte j = 0; j < SIZE; j++) {
      if ((i > 0) && (j > 0)) {
        if (!(b[i][j][3] + b[i][j][2] + b[i - 1][j - 1][0] + b[i - 1][j - 1][1])) {
          switch (random()%4) {
            case 0: b[i][j][3] = 1;      break;
            case 1: b[i][j][2] = 1;      break;
            case 2: b[i - 1][j - 1][0] = 1;  break;
            case 3: b[i - 1][j - 1][1] = 1;  break;
          }
        }
      }
    }
  }
  
  //Clear out center peg walls
  b[LCEN][LCEN][0] = 0;
  b[LCEN][LCEN][1] = 0;
  b[UCEN][LCEN][1] = 0;
  b[UCEN][LCEN][2] = 0;
  b[UCEN][UCEN][2] = 0;
  b[UCEN][UCEN][3] = 0;
  b[LCEN][UCEN][3] = 0;
  b[LCEN][UCEN][0] = 0;

  matchCells(b);
} // end generateRandomMaze

void clearWalls(char b[SIZE][SIZE][5]) {
  //Clean Slate//
  //starts the maze without having any walls anywhere
  for (byte i = 0; i < SIZE; i++) {
    for (byte j = 0; j < SIZE; j++) {
      for (byte k = 0; k < 5; k++) {
        b[i][j][k] = 0;
      }
    }
  }
} // end clearWalls

void randomMaze(char b[SIZE][SIZE][5]) {
  boolean goodGen = false;  //whether or not the maze is solvable
  randomSeed(millis());
  
  //----------//----------//MAZE GENERATION//----------//----------//
  
  while (!goodGen) {  //makes sure maze is solvable
    clearWalls(b);
    generateRandomMaze(b);
    goodGen = floodFill(b, -1, -1);
  }
} // end randomMaze

void printMouseMaze(char b[SIZE][SIZE][5], byte row, byte col, byte dir, boolean flood) {
  //----------//Print Mouse Maze//----------//
  // Prints the maze that the mouse sees
  // Analyzes what walls connect to each peg and prints the corners and walls of the maze
  // Also prints the mouse and its orientation, as well as the flood values if toggled on
  char wall[16] = {(char)32, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35, 
                   (char)32, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35, (char)35};
  int n = 0;
  
  for (byte i = SIZE - 1; i < 255; i--) { // for a byte, -1 = 255, so this will still iterate 16 times
    
    for (byte j = 0; j < SIZE; j++) { // places ceiling of each row
      n = 0;
      if (i < SIZE - 1) { // if not the top row, adds the north wall for each peg
        n = b[i + 1][j][3];
      }
      if (j > 0) { // if not the first column, adds the west wall for each peg
        n = n + 8*b[i][j - 1][0];
      }
      n = n + 2*b[i][j][0] + 4*b[i][j][3]; // sets n to be the number the corner represents
      Serial.print(wall[n]);
      Serial.print(wall[10*b[i][j][0]]);
      Serial.print(wall[10*b[i][j][0]]);
      if (j == SIZE - 1) { // places far left wall
        n = 0;
        if (i < SIZE - 1) { // if not the top row, adds north wall
          n = b[i + 1][j][1];
        }
        n = n + 4*b[i][j][1] + 8*b[i][j][0];
        Serial.print(wall[n]);
      }
    }
    Serial.print("\n");
    
    for (byte j = 0; j < SIZE; j++) { // places walls and spaces of each row
      Serial.print(wall[5*b[i][j][3]]); // prints wall if it exists
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
        Serial.print(wall[5*b[i][j][1]]);
      }
    }
    Serial.print("\n");
    
    if (i == 0) { // if last row, prints the floor as well
      for (byte j = 0; j < SIZE; j++) { // places floor of maze
        n = 0;
        if (j > 0) {
          n = n + 8*b[i][j - 1][2];
        }
        n = n + b[i][j][3] + 2*b[i][j][2];
        Serial.print(wall[n]);
        Serial.print(wall[10*b[i][j][2]]);
        Serial.print(wall[10*b[i][j][2]]);
        if (j == SIZE - 1) { // if last column, prints bottom right corner
          Serial.print(wall[b[i][j][1] + 8*b[i][j][2]]);
        }
      }
      Serial.print("\n");
    }
    
  }
} // end printMouseMaze

void autoPilot(char b[SIZE][SIZE][5], char c[SIZE][SIZE][5], byte &row, byte &col, byte &dir, bool flood, float targetrow, float targetcol) {
  int autodir, autodirnum;
  
  senseWall(dir, row, col, b, c); // senses the initial cell's walls
  matchCells(c);
  
  p[row][col] = true; // cell currently standing in is part of the path
  pchange = false;
  
  while (true) {

    // Floodfills mouse's maze array from different locations depending on the target
    if ((targetrow == UCEN || targetrow == LCEN) && (targetcol == UCEN || targetcol == LCEN)) {
      floodFill(c, -1, -1);
    }
    else {
      floodFill(c, (int)targetrow, (int)targetcol);
    }

    if (c[row][col][4] <= 0) { // if target has been reached or is closed off
      for (int i = LCEN; i <= UCEN; i++) {
        for (int j = LCEN; j <= UCEN; j++) {
          for (int k = 0; k < 4; k++) {
            c[i][j][k] = b[i][j][k]; // set walls of center equal to actual maze walls
          }
        }
      }
      // Prints the maze(s) to the screen
      matchCells(c);
      printFullMaze(b, row, col);
      printMouseMaze(c, row, col, dir, flood);
      return;
    }

    autodir = -1;
    autodirnum = c[row][col][4];
    if (row < SIZE - 1) {
      // If cell above has a lower flood value and is accessible
      if (c[row + 1][col][4] < autodirnum && !b[row][col][0]) {
        autodir = 0;
      }
    }
    if (row > 0) {
      // If cell below ... (same as above)
      if (c[row - 1][col][4] < autodirnum && !b[row][col][2]) {
        autodir = 2;
      }
    }
    if (col < SIZE - 1) {
      // If cell to the right ... (same as the first)
      if (c[row][col + 1][4] < autodirnum && !b[row][col][1]) {
        autodir = 1;
      }
    }
    if (col > 0) {
      // If cell to the left ... (same as the first)
      if (c[row][col - 1][4] < autodirnum && !b[row][col][3]) {
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

    senseWall(dir, row, col, b, c);
    matchCells(c);
    
    if (p[row][col] == false) { // if never been on this cell before, indicate path change
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

// shifts array up and puts 0 in first position by default
void arrayShift(int my_arr[], int next = 0) {
  for(int i = length - 1; i > 0; i--) {
    my_arr[i] = my_arr[i - 1];
    //Serial.print(my_arr[i]);
    //Serial.print(" ");
  }
  my_arr[0] = next;
  //Serial.println(my_arr[0]);
} // end arrayShift

void loop() {
  
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte and put in "buffer"
    arrayShift(input, Serial.read());
    
    // say what you got:
    /*Serial.print("I received: ");
    Serial.write(input[0]);
    Serial.print(", ");
    Serial.println(input[0], DEC);*/
    
    if (input[0] >= 65 && input[0] <= 90) {  // uppercase to lowercase
      input[0] += 32;
    }
    
    // "gen" input
    if (input[2] == 'g' && input[1] == 'e' && input[0] == 'n') {
      
      Serial.println("Generating...");
      randomMaze(g);
      Serial.println("Generated");
      Serial.println("Printing...");
      clearWalls(m);
      
      for (byte i = 0; i < SIZE; i++) {
        for (byte j = 0; j < SIZE; j++) {
            p[i][j] = false;
        }
      }
      
      row = 0;
      col = 0;
      dir = 0;
      floodFill(g, -1, -1);
      printFullMaze(g, row, col);
      printMouseMaze(g, row, col, dir, flood);
      Serial.println("Printed");
      
    }
    
    // "w" input
//    if (input[0] == 'w' && !g[row][col][0]) {  // if no wall, go north
//      moveN(dir, row, col);
//      senseWall(dir, row, col, g, m);
//      matchCells(m);
//      printMouseMaze(m, row, col, dir, flood);
//    }
    
    // "a" input
//    if (input[0] == 'a' && !g[row][col][3]) {  // go west
//      moveW(dir, row, col);
//      senseWall(dir, row, col, g, m);
//      matchCells(m);
//      printMouseMaze(m, row, col, dir, flood);
//    }
    
    // "s" input
//    if (input[0] == 's' && !g[row][col][2]) {  // go south
//      moveS(dir, row, col);
//      senseWall(dir, row, col, g, m);
//      matchCells(m);
//      printMouseMaze(m, row, col, dir, flood);
//    }
    
    // "d" input
//    if (input[0] == 'd' && !g[row][col][1]) {  // go east
//      moveE(dir, row, col);
//      senseWall(dir, row, col, g, m);
//      matchCells(m);
//      printMouseMaze(m, row, col, dir, flood);
//    }
    
    // "f" input
    if (input[0] == 'f') {
      flood  = !flood;
      printMouseMaze(m, row, col, dir, flood);
    }
    
    // "r" input
//    if (input[0] == 'r') { // simulates rotating the mouse to discover all walls
//      for (byte i = 0; i < 4; i++) {
//        senseWall(dir + i, row, col, g, m);
//      }
//      matchCells(m);
//      printMouseMaze(m, row, col, dir, flood);
//    }
    
    // "solve" input
    if (input[4] == 's' && input[3] == 'o' && input[2] == 'l' && input[1] == 'v' && input[0] == 'e') {
      digitalWrite(ledPin, LOW);
      Serial.println("Sure thing.  I'll begin solving right away.  Thanks for asking so politely!");
      
      pchange = true;
      while (pchange) { // repeat going to center and then back to origin until there is no path change
        autoPilot(g, m, row, col, dir, flood, UCEN, UCEN);
        autoPilot(g, m, row, col, dir, flood, 0, 0);
      }
//      digitalWrite(ledPin, HIGH);
      
    }
    
    // "center" input
    if (input[5] == 'c' && input[4] == 'e' && input[3] == 'n' && input[2] == 't' && input[1] == 'e' && input[0] == 'r') {
      Serial.println("Going to center.");
      autoPilot(g, m, row, col, dir, flood, UCEN, UCEN);
    }
    
    // "return" input
    if (input[5] == 'r' && input[4] == 'e' && input[3] == 't' && input[2] == 'u' && input[1] == 'r' && input[0] == 'n') {
      Serial.println("Going to origin.");
      autoPilot(g, m, row, col, dir, flood, 0, 0);
    }
    
    clock = (int)millis();
  }
  
  // if timer expires, reset input array
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
