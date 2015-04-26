void moveN(byte &dir, byte &row, byte &col) {
  switch (dir) {
    case 1:
      turnL(dir);  // turn left 1 time if facing EAST
      break;
    case 2:
      turnR(dir);  // turn right 2 times if facing SOUTH
    case 3:
      turnR(dir);  // turn right 1 time if facing WEST
  }
  moveF(dir, row, col);
}

void moveE(byte &dir, byte &row, byte &col){
  switch (dir) {
    case 2:
      turnL(dir);  // turn left 1 time if facing SOUTH
      break;
    case 3:
      turnR(dir);  // turn right 2 times if facing WEST
    case 0:
      turnR(dir);  // turn right 1 time if facing NORTH
  }
  moveF(dir, row, col);
}

void moveS(byte &dir, byte &row, byte &col) {
  switch (dir) {
    case 3:
      turnL(dir);  // turn left 1 time if facing WEST
      break;
    case 0:
      turnR(dir);  // turn right 2 times if facing NORTH
    case 1:
      turnR(dir);  // turn right 1 time if facing EAST
  }
  moveF(dir, row, col);
}

void moveW(byte &dir, byte &row, byte &col) {
  switch (dir) {
    case 0:
      turnL(dir);  // turn left 1 time if facing NORTH
      break;
    case 1:
      turnR(dir);  // turn right 2 times if facing EAST
    case 2:
      turnR(dir);  // turn right 1 time if facing SOUTH
  }
  moveF(dir, row, col);
}

void moveF(byte dir, byte &row, byte &col) {
  forward(spd);
  switch (dir) {
    case 0: row++; break;
    case 1: col++; break;
    case 2: row--; break;
    case 3: col--; break;
  }
}

void moveB() {

}

void turnR(byte & dir) {
  turn_right(spd);
  dir = (dir + 1)%4;
}

void turnL(byte & dir) {
  turn_left(spd);
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
  while (i < 140) {
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
  while (i < 65) { //alter this value based on the amount of rotation
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

void stationaryDirectionCorrection(float spd) {
//  int reads[2] = {1024, 1024};
//  int maxes[2];
//  int i = 0;
//  int noiseThreshold = 5;
//  
////  while (true) {
//  reads[i] = analogRead(i);
//  maxes[i] = 0;
//  while (reads[i] + noiseThreshold > maxes[i] && reads[i] > 50) {  // attempt to correct by turning right
//    motorR.setSpeed(-spd);
//    motorL.setSpeed(spd);
//    motorR.move(1);
//    motorL.move(-1);
////    for (byte j = 0; j < 1; j++) {
//      motorR.runSpeedToPosition();
//      motorL.runSpeedToPosition();
//      delay(7);
////    }
//    
//    if (reads[i] > maxes[i]) {
//      maxes[i] = reads[i];
//    }
//    reads[i] = analogRead(i);
//    Serial.print(reads[i]);
//    Serial.print(", ");
//    Serial.println(maxes[i]);
//  }
//  Serial.println("end");
//  
//  reads[i] = analogRead(i);
//  maxes[i] = 0;
//  while (reads[i] + noiseThreshold > maxes[i] && reads[i] > 50) {  // attempt to correct by turning left
//    motorR.setSpeed(spd);
//    motorL.setSpeed(-spd);
//    motorR.move(-1);
//    motorL.move(1);
////    for (byte j = 0; j < 1; j++) {
//      motorR.runSpeedToPosition();
//      motorL.runSpeedToPosition();
//      delay(7);
////    }
//    
//    if (reads[i] > maxes[i]) {
//      maxes[i] = reads[i];
//    }
//    reads[i] = analogRead(i);
//  }
////  delay(1000);
////  }
}
