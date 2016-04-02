// wallSet
// Returns the decimal number represented by the inputs as boolean digits
// The inputs are ordered from least to most significant bit
unsigned char wallSet(bool north, bool east, bool south, bool west) {
  return (char)(north + east*2 + south*4 + west*8);
}//end wallSet

// wallExists
// Evaluates the boolean equivalent of 'identifier' for the digit specified by 'dir'
// For direction: N = 0, E = 1, S = 2, W = 3
bool wallExists(unsigned char & identifier, unsigned char dir) {
  for (unsigned char i = 0; i < dir; i++) {
    identifier = identifier/2;
  }
  if (identifier%2) {
    return true;
  }
  return false;
}//end wallExists

// wallChange
// Adds the value of 'addWall' to the digit 'dir' of the boolean equivalent of 'identifier'
// For direction: N = 0, E = 1, S = 2, W = 3
// Includes wall-add/-subtract redundancy checking
// e.g. wallChange(8, 3, true) and wallChange(8, 2, false) will both return 8
void wallChange(unsigned char &identifier, unsigned char dir, bool addWall) {
  if (addWall && !wallExists(identifier, dir)) {  // if adding a wall to an empty space
    switch (dir) {
      case 3: identifier += 4;  // if WEST, add 8
      case 2: identifier += 2;  // if SOUTH, add 4
      case 1: identifier += 1;  // if EAST, add 2
      case 0: identifier += 1;  // if NORTH, add 1
    }
  }
  else if (!addWall && wallExists(identifier, dir)) {  // if removing a wall from a filled space
    switch (dir) {
      case 3: identifier -= 4;  // if WEST, subtract 8
      case 2: identifier -= 2;  // if SOUTH, subtract 4
      case 1: identifier -= 1;  // if EAST, subtract 2
      case 0: identifier -= 1;  // if NORTH, subtract 1
    }
  }
}//end wallChange

// clearWalls
// sets all walls to 0
void clearWalls() {
  for (unsigned char i = 0; i < SIZE; i++) {
    for (unsigned char j = 0; j < SIZE; j++) {
      m[i][j] = 0;
      f[i][j] = 0;
    }
  }
} // end clearWalls

// matchCells
// makes sure each cell has the same walls as the adjacent cells
void matchCells() {
  for (unsigned char i = 0; i < SIZE; i++) {
    for (unsigned char j = 0; j < SIZE; j++) {
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

