void moveN(byte &dir, byte &row, byte &col) {
  if (dir == 0) {
    moveF(dir, row, col);
  }
  else if (dir == 2) {
    turnR(dir);
    turnR(dir);
    moveF(dir, row, col);
  }
  else if (dir == 1) {
    turnL(dir);
    moveF(dir, row, col);
  }
  else if (dir == 3) {
    turnR(dir);
    moveF(dir, row, col);
  }
}

void moveE(byte &dir, byte &row, byte &col){
  if (dir == 1) {
    moveF(dir, row, col);
  }
  else if (dir == 3) {
    turnR(dir);
    turnR(dir);
    moveF(dir, row, col);
  }
  else if (dir == 2) {
    turnL(dir);
    moveF(dir, row, col);
  }
  else if (dir == 0) {
    turnR(dir);
    moveF(dir, row, col);
  }
}

void moveS(byte &dir, byte &row, byte &col) {
  if (dir == 2) {
    moveF(dir, row, col);
  }
  else if (dir == 0) {
    turnR(dir);
    turnR(dir);
    moveF(dir, row, col);
  }
  else if (dir == 3) {
    turnL(dir);
    moveF(dir, row, col);
  }
  else if (dir == 1) {
    turnR(dir);
    moveF(dir, row, col);
  }
}

void moveW(byte &dir, byte &row, byte &col) {
  if (dir == 3) {
    moveF(dir, row, col);
  }
  else if (dir == 1) {
    turnR(dir);
    turnR(dir);
    moveF(dir, row, col);
  }
  else if (dir == 0) {
    turnL(dir);
    moveF(dir, row, col);
  }
  else if (dir == 2) {
    turnR(dir);
    moveF(dir, row, col);
  }
}

void moveF(byte dir, byte &row, byte &col) {
  forward(spd/2);
  if (dir == 0) {
    row++;
  }
  else if (dir == 2) {
    row--;
  }
  else if (dir == 1) {
    col++;
  }
  else if (dir == 3) {
    col--;
  }
}

void moveB() {

}

void turnR(byte & dir) {
  turn_right(spd/2);
  dir = (dir + 1)%4;
}

void turnL(byte & dir) {
  turn_left(spd/2);
  dir = (dir + 3)%4;
}

void forward(float spd) {
  int i = 0;
  motorR.setSpeed(spd*-1);
  motorL.setSpeed(spd*-1);
  if (counterForward >= 4) {
    counterForward -= 4;
    motorR.runSpeed();
    motorL.runSpeed();
  }
  else if (counterForward <= -4) {
    i++;
    counterForward += 4;
  }
  while (i < 143) {
    motorR.runSpeed();
    motorL.runSpeed();
    i++;    
    delay(7);
  }
  counterForward++;
}

void backward(float spd)  {
  forward(spd*-1);
  counterForward -= 2;
}

void turn_right(float spd) {
  int i = 0;
  motorR.setSpeed(spd*-1);
  motorL.setSpeed(spd);
  if (counterTurn > 3) {
    counterTurn -= 4;
    i--;
  }
  else if (counterTurn < -3) {
    counterTurn += 4;
    i++;
  }
  while (i < 66) { //alter this value based on the amount of rotation
    motorR.runSpeed();
    motorL.runSpeed();
    i++;
    delay(7);
  }
  counterTurn++;
}

void turn_left(float spd) {
  turn_right(spd*-1);
  counterTurn--;
  counterTurn--;
}
