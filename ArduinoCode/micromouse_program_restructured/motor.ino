/*
 * Movement/motor controlling
 */

//Movement relative to the maze
//moveN, moveE, moveS, moveW, turnR, turnL all record the movement in the maze and the direction of the mouse
void moveN(unsigned char & dir, unsigned char & row, unsigned char & col, bool finalRun) {
  switch (dir) {
    case 1:
      turnL(dir);  // turn left 1 time if facing EAST
      break;
    case 2:
      turnR(dir);  // turn right 2 times if facing SOUTH
    case 3:
      turnR(dir);  // turn right 1 time if facing WEST
  }
  moveF(dir, row, col, finalRun);
}

void moveE(unsigned char &dir, unsigned char &row, unsigned char &col, bool finalRun){
  switch (dir) {
    case 2:
      turnL(dir);  // turn left 1 time if facing SOUTH
      break;
    case 3:
      turnR(dir);  // turn right 2 times if facing WEST
    case 0:
      turnR(dir);  // turn right 1 time if facing NORTH
  }
  moveF(dir, row, col, finalRun);
}

void moveS(unsigned char &dir, unsigned char &row, unsigned char &col, bool finalRun) {
  switch (dir) {
    case 3:
      turnL(dir);  // turn left 1 time if facing WEST
      break;
    case 0:
      turnR(dir);  // turn right 2 times if facing NORTH
    case 1:
      turnR(dir);  // turn right 1 time if facing EAST
  }
  moveF(dir, row, col, finalRun);
}

void moveW(unsigned char &dir, unsigned char &row, unsigned char &col, bool finalRun) {
  switch (dir) {
    case 0:
      turnL(dir);  // turn left 1 time if facing NORTH
      break;
    case 1:
      turnR(dir);  // turn right 2 times if facing EAST
    case 2:
      turnR(dir);  // turn right 1 time if facing SOUTH
  }
  moveF(dir, row, col, finalRun);
}

void moveF(unsigned char dir, unsigned char &row, unsigned char &col, bool finalRun) {
  if (!finalRun) {  // if on the final run, don't move the motors
    forward(spd);
  }
  switch (dir) {
    case 0: row++; break;
    case 1: col++; break;
    case 2: row--; break;
    case 3: col--; break;
  }
}

void turnR(unsigned char & dir) {
  turn_right(spd);
  dir = (dir + 1)%4;
}

void turnL(unsigned char & dir) {
  turn_left(spd);
  dir = (dir + 3)%4;
}

//end maze relative movement

//Movement relative to mouse
void forward(float spd) {
  unsigned char forwardStep = 139;
  unsigned char i = 0;
  int sensor[5];  // 0 is right, 2 is center, 4 is left
  
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
  while (i < forwardStep) {
    sensor[0] = analogRead(0);
    sensor[1] = analogRead(1);
    sensor[2] = analogRead(2);
    sensor[3] = analogRead(3);
    sensor[4] = analogRead(4);
    
    if (sensor[0] >= 350 || sensor[1] >= 350) {
      motorL.runSpeed();
      delay(7);
    }
    if (sensor[4] >= 350 || sensor[3] >= 350) {
      motorR.runSpeed();
      delay(7);
    }
    
    motorR.setSpeed(spd*-1);
    motorL.setSpeed(spd*-1);
    if (sensor[2] < 750) {
      motorR.runSpeed();
      motorL.runSpeed();
    }
    i++;
    delay(7);
  }
  
  sensor[0] = analogRead(0);
  sensor[2] = analogRead(2);
  sensor[4] = analogRead(4);
  
  if (sensor[0] >= 350) {
    motorL.runSpeed();
  }
  if (sensor[4] >= 350) {
    motorR.runSpeed();
  }
  
  motorR.setSpeed(spd*-1);
  motorL.setSpeed(spd*-1);
  if (sensor[2] < 700) {
    motorR.runSpeed();
    motorL.runSpeed();
  }
  i++;
  counterForward++;
}

void backward(float spd)  {
  forward(spd*-1);
  counterForward -= 2;
}

void turn_right(float spd) {
  char turnStep = 65;
  char i = 0;
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
  while (i < turnStep) { //alter this value based on the amount of rotation
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
