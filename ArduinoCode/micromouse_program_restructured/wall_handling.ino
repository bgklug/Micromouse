char wallSet(bool north, bool east, bool south, bool west) {
  // Returns the decimal number represented by the inputs as boolean digits
  // The inputs are ordered from least to most significant bit
  return (char)(north + east*2 + south*4 + west*8);
}

bool wallExists(byte identifier, byte dir) {
  // Evaluates the boolean equivalent of 'identifier' for the digit specified by 'dir'
  // For direction: N = 0, E = 1, S = 2, W = 3
  for (byte i = 0; i < dir; i++) {
    identifier = identifier/2;
  }
  if (identifier%2) {
    return true;
  }
  return false;
}

void wallChange(byte &identifier, byte dir, bool addWall) {
  // Adds the value of 'addWall' to the digit 'dir' of the boolean equivalent of 'identifier'
  // For direction: N = 0, E = 1, S = 2, W = 3
  // Includes wall-add/-subtract redundancy checking
  //   e.g. wallChange(8, 3, true) and wallChange(8, 2, false) will both return 8
  if (addWall && !wallExists(identifier, dir)) {  // if adding a wall to an empty space
    switch (dir) {
      case 3: identifier += 4;  // if WEST, add 8
      case 2: identifier += 2;  // if SOUTH, add 4
      case 1: identifier += 1;  // if EAST, add 2
      case 0: identifier += 1;  // if NORTH, add 1
    }
  }
  if (!addWall && wallExists(identifier, dir)) {  // if removing a wall from a filled space
    switch (dir) {
      case 3: identifier -= 4;  // if WEST, subtract 8
      case 2: identifier -= 2;  // if SOUTH, subtract 4
      case 1: identifier -= 1;  // if EAST, subtract 2
      case 0: identifier -= 1;  // if NORTH, subtract 1
    }
  }
}

