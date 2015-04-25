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
  dir = (dir + 1)%4;
}

void turnL(byte & dir){
  dir = (dir + 3)%4;
}
