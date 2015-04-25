void moveN(byte & dir, byte & mouRow, byte & mouCol){
  if(dir == 0){
    moveF(dir, mouRow,mouCol);
  }else if(dir == 2){
    turnR(dir);
    turnR(dir);
    moveF(dir, mouRow,mouCol);
  }else if(dir == 1){
    turnL(dir);
    moveF(dir, mouRow,mouCol);
  }else if(dir == 3){
    turnR(dir);
    moveF(dir, mouRow,mouCol);
  }
}

void moveE(byte & dir, byte & mouRow, byte & mouCol){
  if(dir == 1){
    moveF(dir, mouRow,mouCol);
  }else if(dir == 3){
    turnR(dir);
    turnR(dir);
    moveF(dir, mouRow,mouCol);
  }else if(dir == 2){
    turnL(dir);
    moveF(dir, mouRow,mouCol);
  }else if(dir == 0){
    turnR(dir);
    moveF(dir, mouRow,mouCol);
  }
}

void moveS(byte & dir, byte & mouRow, byte & mouCol){
  if(dir == 2){
    moveF(dir, mouRow,mouCol);
  }else if(dir == 0){
    turnR(dir);
    turnR(dir);
    moveF(dir, mouRow,mouCol);
  }else if(dir == 3){
    turnL(dir);
    moveF(dir, mouRow,mouCol);
  }else if(dir == 1){
    turnR(dir);
    moveF(dir, mouRow,mouCol);
  }
}

void moveW(byte & dir, byte & mouRow, byte & mouCol){
  if(dir == 3){
    moveF(dir, mouRow,mouCol);
  }else if(dir == 1){
    turnR(dir);
    turnR(dir);
    moveF(dir, mouRow,mouCol);
  }else if(dir == 0){
    turnL(dir);
    moveF(dir, mouRow,mouCol);
  }else if(dir == 2){
    turnR(dir);
    moveF(dir, mouRow,mouCol);
  }
}

void moveF(byte dir, byte & mouRow, byte & mouCol){
  forward(spd/2);
  if(dir == 0){
    ++mouRow;
  }else if(dir == 2){
    --mouRow;
  }else if(dir == 1){
    ++mouCol;
  }else if(dir == 3){
    --mouCol;
  }
}

void moveB(){

}

void turnR(byte & dir){
  turn_right(spd/2);
  dir = (dir + 1)%4;
}

void turnL(byte & dir){
  turn_left(spd/2);
  dir = (dir + 3)%4;
}





void forward(float spd)
{
  int i=0;
  motorR.setSpeed(spd*-1);
  motorL.setSpeed(spd*-1);
  if(counterForward >= 4)
  {
    counterForward -= 4;
    motorR.runSpeed();
    motorL.runSpeed();
  }
  else if(counterForward <= -4)
  {
    i++;
   counterForward +=4; 
  }
  while (i<143)
  {
    
    motorR.runSpeed();
    motorL.runSpeed();
    i++;    
    delay(7);
  }
  counterForward++;
}

void backward(float spd)
{
  forward(spd*-1);
  counterForward -= 2;
}

void turn_right(float spd)
{
  int i=0;
  motorR.setSpeed(spd*-1);
  motorL.setSpeed(spd);
  if(counterTurn > 3)
  {
    counterTurn -=4;
    i--;
  }
  else if(counterTurn < -3)
  {
    counterTurn +=4;
    i++;
  }
  while(i<66) //alter this value based on the amount of rotation
  {
    motorR.runSpeed();
    motorL.runSpeed();
    i++;
    delay(7);
  }
  counterTurn++;
}

void turn_left(float spd)
{
  turn_right(spd*-1);
  counterTurn--;
  counterTurn--;
}
