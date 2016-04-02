#define STEP_D   1.257          // Distance traveled in millimeters per motor step.
#define STEP_B1  28             // Step count for backing up (during calibration).
#define DELTA_D  STEP_D*STEP_B1 // Delta distance in millimeters traveled during callibration.
#define WALL_F   90             // Distance in millimeters the bot can sense a forward wall.
#define WALL_S   60             // Distance in millimeters the bot can sense a right wall.
#define MOTOR_F  25             // Distance in millimeters away from a forward wall before stopping.
#define MOTOR_S  35             // Distance in millimeters away from a right wall before correcting while moving.
#define MOTOR_D  35             // Distance in millimeters away from a diagonal wall before correcting while moving.
#define AVECOUNT 3              // Number of sensor readings to average together (during senseWall).
#define DELAY_S  1              // Delay in milliseconds between each wall sensor reading (during senseWall).
#define DELAY_C  0              // Delay in milliseconds between centering and anything else.

int debug = 0;

int centerUL;
int centerUR;
int wallValL;
int wallValF;
int wallValR;
int motorValL;
int motorValF;
int motorValR;
int motorValDL;
int motorValDR;


// Calibrates global sensor values.
void calibrateSensors (byte & dir) {
  moveB(STEP_B1);
  
  motorValL = analogRead(A2);
  motorValF = analogRead(A1);
  motorValR = analogRead(A0);
  digitalWrite(6, HIGH);
  delay(DELAY_S);
  motorValDR = analogRead(A3);
  digitalWrite(6, LOW);
  delay(DELAY_S);
  motorValDL = analogRead(A3);

  Serial.print("L: ");  Serial.println(motorValL);
  Serial.print("F: ");  Serial.println(motorValF);
  Serial.print("R: ");  Serial.println(motorValR);
  Serial.print("LD: ");  Serial.println(motorValDL);
  Serial.print("RD: ");  Serial.println(motorValDR);

  Serial.println();
  Serial.print("Do: ");  Serial.println(DELTA_D);
  
  unsigned long K = pow(DELTA_D,2) * motorValF;

  Serial.print("K: ");  Serial.println(K);
  
  wallValL = max(0,(K / (pow(WALL_S, 2))));
  wallValF = max(0,(K / (pow(WALL_F, 2))));
  wallValR = max(0,(K / (pow(WALL_S, 2))));
  motorValL = min(1026,(K / (pow(MOTOR_S, 2))));
  motorValF = min(1026,(K / (pow(MOTOR_F, 2))));
  motorValR = min(1026,(K / (pow(MOTOR_S, 2))));
  motorValDL = min(1026,(K / (pow(MOTOR_D, 2))));
  motorValDR = min(1026,(K / (pow(MOTOR_D, 2))));

  Serial.println();
  Serial.print("WallF: " ); Serial.println(wallValF);
  Serial.print("MotorF: " ); Serial.println(motorValF);
  Serial.println();
  Serial.print("WallS: " ); Serial.println(wallValL);
  Serial.print("MotorS: " ); Serial.println(motorValL);
  Serial.println();
  Serial.print("WallD: " ); Serial.println(motorValDL);
  Serial.print("MotorD: " ); Serial.println(motorValDL);

  delay(1000);
  
  centerF();
  turnR(dir); turnR(dir);
} // end calibrateSensors

// Senses wall while holding still and changes the mouse's maze. Should be ran in every unexplored cell.
void senseWall(char &mou, byte dir, byte row, byte col){
  int wallAveL = 0;
  int wallAveF = 0;
  int wallAveR = 0;
  // Serial.println("\nReadings:");
  for (int i=0; i<AVECOUNT; i++){
    wallAveL += analogRead(A2);  // Serial.print("L: "); Serial.print(analogRead(A2));
    wallAveF += analogRead(A1);  // Serial.print("   F: "); Serial.print(analogRead(A1));
    wallAveR += analogRead(A0);  // Serial.print("   R: "); Serial.println(analogRead(A0));
    delay(DELAY_S);
  }
  wallAveL = wallAveL / AVECOUNT;
  wallAveF = wallAveF / AVECOUNT;
  wallAveR = wallAveR / AVECOUNT;
  wallChange(mou, (dir + 3)%4, (wallAveL > wallValL)); // Left-Side
  wallChange(mou, dir, (wallAveF > wallValF));         // Forward
  wallChange(mou, (dir + 1)%4, (wallAveR > wallValR)); // Right-Side
  /*
  Serial.println("Average:");
  Serial.print("L: ");  Serial.print(wallAveL);  //Left
  Serial.print("   F: ");  Serial.print(wallAveF);  //Forward
  Serial.print("   R: ");  Serial.println(wallAveR);  //Right  
  */
} // end senseWall


// Senses walls while moving and controlls the motors. Should be ran every step.
// Consider having another function to find highest sensor value, then use a case statement.
boolean senseMotor(){
  /*
  if (debug > 99){
    Serial.println("senseMotor:");
    Serial.print("L: ");  Serial.println(analogRead(A2));             //Left
    Serial.print("F: ");  Serial.println(analogRead(A1));             //Forward
    Serial.print("R: ");  Serial.println(analogRead(A0));             //Right
    if (select){Serial.print("DL: ");} else { Serial.print("DR: ");}  
    Serial.println(analogRead(A3));                                   //Diagonal
    debug = 0;
  } else { debug++; }
  */
  if(analogRead(A1) > motorValF){ return true; }                                                 // Forward
  if((analogRead(A2) > motorValL) && (analogRead(A0) < motorValR)){ motorL->onestep(FORWARD, DOUBLE); return true; }               // Left-Side
  if((analogRead(A0) > motorValR) && (analogRead(A2) < motorValL)){ motorR->onestep(FORWARD, DOUBLE); return true; }               // Right-Side
  if((analogRead(A3) > motorValDL) &&  select){ motorL->onestep(FORWARD, DOUBLE); return true; } // Left-Diagonal
  if((analogRead(A3) > motorValDR) && !select){ motorR->onestep(FORWARD, DOUBLE); return true; } // Right-Diagonal
  return false;
} // end senseMotor


// senseMotor without diagonal sensors, for centerF.
boolean senseMotorND(){
  if(analogRead(A1) > motorValF){ return true; }                                                 // Forward
  if((analogRead(A2) > motorValL) && (analogRead(A0) < motorValR)){ motorL->onestep(FORWARD, DOUBLE); return true; }               // Left-Side
  if((analogRead(A0) > motorValR) && (analogRead(A2) < motorValL)){ motorR->onestep(FORWARD, DOUBLE); return true; }               // Right-Side
  return false;
} // end senseMotor


void centerF(){
  while (analogRead(A1) < motorValF){
    motorL->onestep(FORWARD, DOUBLE);
    motorR->onestep(FORWARD, DOUBLE); 
    senseMotorND();
  }
  delay(DELAY_C);
} // end centerF


void centerUpre(){
  centerUL = analogRead(A2);
  centerUR = analogRead(A0);
  return;
} // end centerUpre


void centerUpost(){
  while (analogRead(A2)>centerUL && analogRead(A0)<centerUR){
    motorL->onestep(FORWARD, DOUBLE);
    motorR->onestep(BACKWARD, DOUBLE); 
  }
  while (analogRead(A2)<centerUL && analogRead(A0)>centerUR){
    motorL->onestep(BACKWARD, DOUBLE);
    motorR->onestep(FORWARD, DOUBLE);
  }
} // end centerUpost
