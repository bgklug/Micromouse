#define STEP_F_MAX  148  // Maximum step count for moving forward.
#define STEP_F_MIN  144  // Minimum step count for moving forward.
#define STEP_T      66   // Step count for turning.
#define DELAY_M     0    // Delay in milliseconds between moving forward and anything else.
#define DELAY_T     100  // Delay in milliseconds between turning and anything else.

byte stepF = STEP_F_MAX;

void moveN(byte & dir, byte & mouRow, byte & mouCol){
  switch (dir) {
    case 0: moveF(dir,mouRow,mouCol); if(stepF>STEP_F_MIN){stepF--;} break;
    case 1: turnL(dir); moveF(dir,mouRow,mouCol); stepF=STEP_F_MAX; break;
    case 2: centerUpre(); turnR(dir); turnR(dir); /*if(wallExists(m[mouRow][mouCol],(dir+1)%4)&&wallExists(m[mouRow][mouCol],(dir+3)%4)){centerUpost();}*/ moveF(dir,mouRow,mouCol); stepF=STEP_F_MAX; break;
    case 3: turnR(dir); moveF(dir,mouRow,mouCol); stepF=STEP_F_MAX; break;
  }
} // end moveN


void moveE(byte & dir, byte & mouRow, byte & mouCol){
  switch (dir) {
    case 1: moveF(dir,mouRow,mouCol); if(stepF>STEP_F_MIN){stepF--;} break;
    case 2: turnL(dir); moveF(dir,mouRow,mouCol); stepF=STEP_F_MAX; break;
    case 3: centerUpre(); turnR(dir); turnR(dir); /*if(wallExists(m[mouRow][mouCol],(dir+1)%4)&&wallExists(m[mouRow][mouCol],(dir+3)%4)){centerUpost();}*/ moveF(dir,mouRow,mouCol); stepF=STEP_F_MAX; break;
    case 0: turnR(dir); moveF(dir,mouRow,mouCol); stepF=STEP_F_MAX; break;
  }
} // end moveE


void moveS(byte & dir, byte & mouRow, byte & mouCol){
  switch (dir) {
    case 2: moveF(dir,mouRow,mouCol); if(stepF>STEP_F_MIN){stepF--;} break;
    case 3: turnL(dir); moveF(dir,mouRow,mouCol); stepF=STEP_F_MAX; break;
    case 0: centerUpre(); turnR(dir); turnR(dir); /*if(wallExists(m[mouRow][mouCol],(dir+1)%4)&&wallExists(m[mouRow][mouCol],(dir+3)%4)){centerUpost();}*/ moveF(dir,mouRow,mouCol); stepF=STEP_F_MAX; break;
    case 1: turnR(dir); moveF(dir,mouRow,mouCol); stepF=STEP_F_MAX; break;
  }
} // end moveS


void moveW(byte & dir, byte & mouRow, byte & mouCol){
  switch (dir) {
    case 3: moveF(dir, mouRow,mouCol); if(stepF>STEP_F_MIN){stepF--;} break;
    case 0: turnL(dir); moveF(dir,mouRow,mouCol); stepF=STEP_F_MAX; break;
    case 1: centerUpre(); turnR(dir); turnR(dir); /*if(wallExists(m[mouRow][mouCol],(dir+1)%4)&&wallExists(m[mouRow][mouCol],(dir+3)%4)){centerUpost();}*/ moveF(dir,mouRow,mouCol); stepF=STEP_F_MAX; break;
    case 2: turnR(dir); moveF(dir,mouRow,mouCol); stepF=STEP_F_MAX; break;
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
  while (stepCount < stepF) {
    if (!senseMotor()) {
      motorL->onestep(FORWARD, DOUBLE);
      motorR->onestep(FORWARD, DOUBLE); 
    }
    stepCount++;
    if(select){
      digitalWrite(6, HIGH);
      select = false;
    }else{
      digitalWrite(6, LOW);
      select = true;
    }
  }
  delay(DELAY_M);
} // end moveF


void turnR(byte & dir){
  dir = (dir + 1)%4;
  unsigned char stepCount = 0;
  while (stepCount < STEP_T) { //alter this value based on the amount of rotation
    motorL->onestep(FORWARD, DOUBLE);
    motorR->onestep(BACKWARD, DOUBLE);
    stepCount++;
  }
  delay(DELAY_T);
} // end turnR


void turnL(byte & dir){
  dir = (dir + 3)%4;
  unsigned char stepCount = 0;
  while (stepCount < STEP_T) { //alter this value based on the amount of rotation
    motorL->onestep(BACKWARD, DOUBLE);
    motorR->onestep(FORWARD, DOUBLE);
    stepCount++;
  }
  delay(DELAY_T);
} // end turnL


void walkF(byte steps){
  char stepCount = 0;
  while (stepCount < steps) {
    motorL->onestep(FORWARD, DOUBLE);
    motorR->onestep(FORWARD, DOUBLE);
    stepCount++;
  }
  delay(DELAY_M);
} // end moveB


void walkB(byte steps){
  char stepCount = 0;
  while (stepCount < steps) {
    motorL->onestep(BACKWARD, DOUBLE);
    motorR->onestep(BACKWARD, DOUBLE);
    stepCount++;
  }
  delay(DELAY_M);
} // end moveB


void rotateL(byte steps){
  char stepCount = 0;
  while (stepCount < steps) {
    motorL->onestep(BACKWARD, DOUBLE);
    motorR->onestep(FORWARD, DOUBLE);
    stepCount++;
  }
  delay(DELAY_T);
} // end rotateL


void rotateR(byte steps){
  char stepCount = 0;
  while (stepCount < steps) {
    motorL->onestep(FORWARD, DOUBLE);
    motorR->onestep(BACKWARD, DOUBLE);
    stepCount++;
  }
  delay(DELAY_T);
} // end rotateR
