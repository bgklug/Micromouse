char g[16][16][5] = { 0 };  // generated maze and flood values
char m[16][16][5] = { 0 };  // mouse maze and flood values
byte row = 0;  // mouse global position values
byte col = 0;

void setup() {
  // begin serial communications at 9600 bits of data per second
  // for communicating with the computer, use one of these rates: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200
  Serial.begin(9600);
}

void printFullMaze(char b[16][16][5], byte row, byte col) {
  //----------//Print Full Maze//----------//
  // Prints a succinct version of the full maze
  Serial.print(" _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _");
  for (byte i = 15; i < 255; i--) {  // byte is unsigned, so -1 is actually 255; still iterates 16 times
    Serial.print("\n|");
    for (byte j = 0; j < 16; j++) {
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

boolean floodFill(char b[16][16][5], char row, char col) {
  //----------//Flood//----------//
  //fills all flood array spaces with -1
  for(byte i = 0; i < 16; i++) {
    for(byte j = 0; j < 16; j++) {
      b[i][j][4] = -1;
    }
  }

  //fills the four goal flood array spaces with 0 by default
  if (row < 0 || col < 0) {
    b[7][7][4] = 0;
    b[7][8][4] = 0;
    b[8][7][4] = 0;
    b[8][8][4] = 0;
  }
  else {
    b[row][col][4] = 0;
  }

  //fills the flood array with values using flood fill logic
  for (int k = 0; k < 256; k++) {  //byte's range is 0-255, so int will stop infinite loop from being created
    for (byte i = 0; i < 16; i++) {
      for (byte j = 0; j < 16; j++) {
        if (b[i][j][4] == k) {  //if the flood array space equals k (starting at 0), place k+1 in adjacent flood array spaces
          if (i < 15) {
            if (!b[i + 1][j][2] && b[i + 1][j][4] == -1) {  //North
              b[i + 1][j][4] = b[i][j][4] + 1;
            }
          }
          if (i > 0) {
            if (!b[i - 1][j][0] && b[i - 1][j][4] == -1) {  //South
              b[i - 1][j][4] = b[i][j][4] + 1;
            }
          }
          if (j < 15) {
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
  if (b[0][0][4] != -1 && b[0][15][4] != -1 && b[15][0][4] != -1 && b[15][15][4] != -1) {
    return 1;
  }
  return 0;
} // end floodFill

void matchCells(char b[16][16][5]) {
  //Matching//
  //makes sure each cell has the same walls as the adjacent cells
  for (byte i = 0; i < 16; i++) {
    for (byte j = 0; j < 16; j++) {
      if (i > 0) {
        if (b[i - 1][j][0]) {
          b[i][j][2] = 1;
        }
        else if (b[i][j][2]) {
          b[i - 1][j][0] = 1;
        }
      }
      if (i < 15) {
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
      if (j < 15) {
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

void generateRandomMaze(char b[16][16][5]) {
  byte percent = 30; // percent chance of placing a wall
  //----------//Walls//----------//
  //build a random maze
  for (byte i = 0; i < 16; i++) {
    for (byte j = 0; j < 16; j++) {
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
  for (byte i = 7; i <= 8; i++) {
    for (byte j = 7; j <= 8; j++) {
      for (byte k = 0; k < 4; k++) {
        b[i][j][k] = 1;
      }
    }
  }

  //Center Opening//
  //places center wall opening
  switch (random()%8) {
    case 0: b[7][7][3] = 0; break;
    case 1: b[7][7][2] = 0; break;
    case 2: b[7][8][2] = 0; break;
    case 3: b[7][8][1] = 0; break;
    case 4: b[8][8][1] = 0; break;
    case 5: b[8][8][0] = 0; break;
    case 6: b[8][7][0] = 0; break;
    case 7: b[8][7][3] = 0; break;
  }

  //Border//
  //make the edges have walls
  for (byte i = 0; i < 16; i++) {
    b[0][i][2] = 1;
    b[15][i][0] = 1;
    b[i][0][3] = 1;
    b[i][15][1] = 1;
  }
  
  //Corners//
  //places the starting positions in the four corners
  b[0][0][0] = 0;
  b[0][0][1] = 1;
  b[0][0][2] = 1;
  b[0][0][3] = 1;
  b[0][1][3] = 1;
  b[1][0][2] = 0;
  b[0][15][0] = 1;
  b[0][15][1] = 1;
  b[0][15][2] = 1;
  b[0][15][3] = 0;
  b[0][14][1] = 0;
  b[1][15][2] = 1;
  b[15][0][0] = 1;
  b[15][0][1] = 0;
  b[15][0][2] = 1;
  b[15][0][3] = 1;
  b[15][1][3] = 0;
  b[14][0][0] = 1;
  b[15][15][0] = 1;
  b[15][15][1] = 1;
  b[15][15][2] = 0;
  b[15][15][3] = 1;
  b[15][14][1] = 1;
  b[14][15][0] = 0;

  matchCells(b);

  //Pegs//
  //makes sure a wall is on every peg
  for (byte i = 0; i < 16; i++) {
    for (byte j = 0; j < 16; j++) {
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
  b[7][7][0] = 0;
  b[7][7][1] = 0;
  b[8][7][1] = 0;
  b[8][7][2] = 0;
  b[8][8][2] = 0;
  b[8][8][3] = 0;
  b[7][8][3] = 0;
  b[7][8][0] = 0;

  matchCells(b);
} // end generateRandomMaze

void clearWalls(char b[16][16][5]) {
  //Clean Slate//
  //starts the maze without having any walls anywhere
  for (byte i = 0; i < 16; i++) {
    for (byte j = 0; j < 16; j++) {
      for (byte k = 0; k < 4; k++) {
        b[i][j][k] = 0;
      }
    }
  }
} // end clearWalls

void randomMaze(char b[16][16][5]) {
  boolean goodGen = false;  //whether or not the maze is solvable
  randomSeed(millis());
  
  //----------//----------//MAZE GENERATION//----------//----------//
  
  while (!goodGen) {  //makes sure maze is solvable
    clearWalls(b);
    generateRandomMaze(b);
    goodGen = floodFill(b, -1, -1);
  }
} // end randomMaze

void printMouseMaze(char b[16][16][5], byte row, byte col, byte dir, boolean flood) {
  //----------//Print Mouse Maze//----------//
  // Prints the maze that the mouse sees
  // Analyzes what walls connect to each peg and prints the corners and walls of the maze
  // Also prints the mouse and its orientation, as well as the flood values if toggled on
  char wall[16] = {(char)32, (char)32, (char)32, (char)192, (char)32, (char)179, (char)218, (char)195, 
                   (char)32, (char)217, (char)196, (char)193, (char)191, (char)180, (char)194, (char)197};
  int n = 0;
  
  for (byte i = 15; i < 255; i--) { // for a byte, -1 = 255, so this will still iterate 16 times
    
    for (byte j = 0; j < 16; j++) { // places ceiling of each row
      n = 0;
      if (i < 15) { // if not the top row, adds the north wall for each peg
        n = b[i + 1][j][3];
      }
      if (j > 0) { // if not the first column, adds the west wall for each peg
        n = n + 8*b[i][j - 1][0];
      }
      n = n + 2*b[i][j][0] + 4*b[i][j][3]; // sets n to be the number the corner represents
      Serial.print(wall[n]);
      Serial.print(wall[10*b[i][j][0]]);
      Serial.print(wall[10*b[i][j][0]]);
      if (j == 15) { // places far left wall
        n = 0;
        if (i < 15) { // if not the top row, adds north wall
          n = b[i + 1][j][1];
        }
        n = n + 4*b[i][j][1] + 8*b[i][j][0];
        Serial.print(wall[n]);
      }
    }
    Serial.print("\n");
    
    for (byte j = 0; j < 16; j++) { // places walls and spaces of each row
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
          Serial.print(b[i][j][4]);
        }
        else {
          Serial.print(b[i][j][4]);
        }
      }
      else { // else, prints a space
        Serial.print("  ");
      }
      if (j == 15) {
        Serial.print(wall[5*b[i][j][1]]);
      }
    }
    Serial.print("\n");
    
    if (i == 0) { // if last row, prints the floor as well
      for (byte j = 0; j < 16; j++) { // places floor of maze
        n = 0;
        if (j > 0) {
          n = n + 8*b[i][j - 1][2];
        }
        n = n + b[i][j][3] + 2*b[i][j][2];
        Serial.print(wall[n]);
        Serial.print(wall[10*b[i][j][2]]);
        Serial.print(wall[10*b[i][j][2]]);
        if (j == 15) { // if last column, prints bottom right corner
          Serial.print(wall[b[i][j][1] + 8*b[i][j][2]]);
        }
      }
      Serial.print("\n");
    }
    
  }
} // end printMouseMaze

const int length = 5;  // predefined input array length
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
      printFullMaze(g, row, col);
      Serial.println("Printed");
      
    }
    
    // "w" input
    if (input[0] == 'w') {
      row++;
      printFullMaze(g, row, col);
    }
    
    // "a" input
    if (input[0] == 'a') {
      col--;
      printFullMaze(g, row, col);
    }
    
    // "s" input
    if (input[0] == 's') {
      row--;
      printFullMaze(g, row, col);
    }
    
    // "d" input
    if (input[0] == 'd') {
      col++;
      printFullMaze(g, row, col);
    }
    
    // "solve" input
    if (input[4] == 's' && input[3] == 'o' && input[2] == 'l' && input[1] == 'v' && input[0] == 'e') {
      Serial.println("Sure thing.  I'll begin solving right away.  Thanks for asking so politely!");
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
  
} // end loop
