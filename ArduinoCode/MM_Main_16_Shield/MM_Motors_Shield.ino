#define STEP_F 150   //Step count for moving forward.
#define STEP_T 66    //Step count for turning.
#define DELAY_M 0     //delay in milliseconds between each motor step.
#define DELAY_F 100  //delay in milliseconds between moving forward and anything else.
#define DELAY_T 100  //delay in milliseconds between turning and anything else.


void moveN(byte & dir, byte & mouRow, byte & mouCol){
  switch (dir) {
    case 0: moveF(dir,mouRow,mouCol); break;
    case 1: turnL(dir); moveF(dir,mouRow,mouCol); break;
    case 2: turnR(dir); turnR(dir); moveF(dir,mouRow,mouCol); break;
    case 3: turnR(dir); moveF(dir,mouRow,mouCol); break;
  }
} // end moveN


void moveE(byte & dir, byte & mouRow, byte & mouCol){
  switch (dir) {
    case 1: moveF(dir,mouRow,mouCol); break;
    case 2: turnL(dir); moveF(dir,mouRow,mouCol); break;
    case 3: turnR(dir); turnR(dir); moveF(dir,mouRow,mouCol); break;
    case 0: turnR(dir); moveF(dir,mouRow,mouCol); break;
  }
} // end moveE


void moveS(byte & dir, byte & mouRow, byte & mouCol){
  switch (dir) {
    case 2: moveF(dir,mouRow,mouCol); break;
    case 3: turnL(dir); moveF(dir,mouRow,mouCol); break;
    case 0: turnR(dir); turnR(dir); moveF(dir,mouRow,mouCol); break;
    case 1: turnR(dir); moveF(dir,mouRow,mouCol); break;
  }
} // end moveS


void moveW(byte & dir, byte & mouRow, byte & mouCol){
  switch (dir) {
    case 3: moveF(dir, mouRow,mouCol); break;
    case 0: turnL(dir); moveF(dir,mouRow,mouCol); break;
    case 1: turnR(dir); turnR(dir); moveF(dir,mouRow,mouCol); break;
    case 2: turnR(dir); moveF(dir,mouRow,mouCol); break;
  }
} // end moveW


void moveF(byte dir, byte & mouRow, byte & mouCol){
  switch (dir) {
  case 0: ++mouRow; break;
  case 1: ++mouCol; break;
  case 2: --mouRow; break;
  case 3: --mouCol; break;
  }
  unsigned char stepCount = 0;
  while (stepCount < STEP_F) {
    if (!senseMotor()) {
      motorL->step(1, FORWARD, SINGLE);
      motorR->step(1, FORWARD, SINGLE); 
    }
    delay(DELAY_M);
    stepCount++;
    if(select){
      digitalWrite(6, HIGH);
      select = false;
    }else{
      digitalWrite(6, LOW);
      select = true;
    }
  }
  delay(DELAY_F);
} // end moveF


void turnR(byte & dir){
  dir = (dir + 1)%4;
  char stepCount = 0;
  while (stepCount < STEP_T) { //alter this value based on the amount of rotation
    motorL->step(1, FORWARD, SINGLE);
    motorR->step(1, BACKWARD, SINGLE);
    stepCount++;
    delay(DELAY_M);
  }
  delay(DELAY_T);
} // end turnR


void turnL(byte & dir){
  dir = (dir + 3)%4;
  char stepCount = 0;
  while (stepCount < STEP_T) { //alter this value based on the amount of rotation
    motorL->step(1, BACKWARD, SINGLE);
    motorR->step(1, FORWARD, SINGLE);
    stepCount++;
    delay(DELAY_M);
  }
  delay(DELAY_T);
} // end turnL
