#define DELTA_D  1.257                // Distance traveled in millimeters per motor step.
#define STEP_B   31                   // Step count for backing up (during calibration).
#define DIST_F   DELTA_D*STEP_B //<40 // Distance in millimeters traveled backwards from the wall during callibration.
#define DIST_S   38.5                 // Distance in millimeters from side sensors to wall during callibration.
#define DIST_D   (DIST_F+11.5)*1.414  // Distance in millimeters from diagonal sensors to wall during callibration.

#define WALL_F   70                   // Distance in millimeters the bot can sense a forward wall.
#define WALL_S   55                   // Distance in millimeters the bot can sense a right wall.
#define MOTOR_F  28.5                 // Distance in millimeters away from a forward wall before stopping.
#define MOTOR_S  35.5                 // Distance in millimeters away from a right wall before correcting while moving.
#define MOTOR_D  40                   // Distance in millimeters away from a diagonal wall before correcting while moving.

#define POW_F    1.52
#define POW_R    1.44
#define POW_L    1.30
#define POW_DR   1.5
#define POW_DL   1.5

//#define DELTA_D  1.257                // Distance traveled in millimeters per motor step.
//#define STEP_B1  20
//#define STEP_B2  11
//#define STEP_S   5
//#define THETA_S  DELTA_D*STEP_S/52
//#define DIST_F1  DELTA_D*STEP_B1
//#define DIST_F2  DELTA_D*(STEP_B1+STEP_B2)
//#define DIST_S1  (38.5 - (2*63.64*sin((PI/4)-THETA_S)*sin(((PI/4)-THETA_S)/2)) )/cos(-THETA_S)
//#define DIST_S2  38.5
//#define DIST_D1  (DIST_F1+11.5)*1.414
//#define DIST_D2  (DIST_F2+11.5)*1.414
//
//#define WALL_F   84                   // Distance in millimeters the bot can sense a forward wall.
//#define WALL_S   77                   // Distance in millimeters the bot can sense a right wall.
//#define MOTOR_F  10                   // Distance in millimeters away from a forward wall before stopping.
//#define MOTOR_S  35.5                 // Distance in millimeters away from a right wall before correcting while moving.
//#define MOTOR_D  40                   // Distance in millimeters away from a diagonal wall before correcting while moving.

#define AVECOUNT 3                    // Number of sensor readings to average together (during senseWall).
#define DELAY_S  1                    // Delay in milliseconds between each wall sensor reading (during senseWall).
#define DELAY_C  0                    // Delay in milliseconds between centering and anything else.

short centerUL;
short centerUR;
short wallValL;
short wallValF;
short wallValR;
short motorValL;
short motorValF;
short motorValR;
short motorValDL;
short motorValDR;


// Calibrates global sensor values.
void calibrateSensors (byte & dir) {
  walkB(STEP_B);

  Serial.print("DF: ");  Serial.println(DIST_F);
  Serial.print("DS: ");  Serial.println(DIST_S);
  Serial.print("DD: ");  Serial.println(DIST_D);

  motorValF = analogRead(A1);
  motorValL = analogRead(A2);
  motorValR = analogRead(A0);
  digitalWrite(6, HIGH);
  delay(DELAY_S);
  motorValDR = analogRead(A3);
  digitalWrite(6, LOW);
  delay(DELAY_S);
  motorValDL = analogRead(A3);

  Serial.println();
  Serial.print("F: ");  Serial.println(motorValF);
  Serial.print("R: ");  Serial.println(motorValR);
  Serial.print("L: ");  Serial.println(motorValL);
  Serial.print("DL: ");  Serial.println(motorValDL);
  Serial.print("DR: ");  Serial.println(motorValDR);
  
  long KF = pow(DIST_F, POW_F) * motorValF; // constant K.
  long KR = pow(DIST_S, POW_R) * motorValR;
  long KL = pow(DIST_S, POW_L) * motorValL;
  long KDL = pow(DIST_D, POW_DL) * motorValDL;
  long KDR = pow(DIST_D, POW_DR) * motorValDR;

  Serial.println();
  Serial.print("KF: ");  Serial.println(KF);
  Serial.print("KR: ");  Serial.println(KR);
  Serial.print("KL: ");  Serial.println(KL);
  Serial.print("KDL: ");  Serial.println(KDL);
  Serial.print("KDR: ");  Serial.println(KDR);
  
  wallValF = max(0, min(1026, KF / (pow(WALL_F, POW_F))));
  wallValR = max(0, min(1026, KR / (pow(WALL_S, POW_R))));
  wallValL = max(0, min(1026, KL / (pow(WALL_S, POW_L))));
  motorValF = max(0, min(1026, KF / (pow(MOTOR_F, POW_F))));
  motorValR = max(0, min(1026, KR / (pow(MOTOR_S, POW_R))));
  motorValL = max(0, min(1026, KL / (pow(MOTOR_S, POW_L))));
  motorValDR = max(0, min(1026, KDR / (pow(MOTOR_D, POW_L))));
  motorValDL = max(0, min(1026, KDL / (pow(MOTOR_D, POW_R))));

  Serial.println();
  Serial.print("WF: " ); Serial.println(wallValF);
  Serial.print("WR: " ); Serial.println(wallValR);
  Serial.print("WL: " ); Serial.println(wallValL);

  Serial.println();
  Serial.print("MF: " ); Serial.println(motorValF);
  Serial.print("MR: " ); Serial.println(motorValR);
  Serial.print("ML: " ); Serial.println(motorValL);
  Serial.print("MDL: " ); Serial.println(motorValDL);
  Serial.print("MDR: " ); Serial.println(motorValDR);

  delay(1000);
  centerF();
  turnR(dir); turnR(dir);
} // end calibrateSensors


//// Calibrates global sensor values.
//void calibrateSensors (byte & dir) {
//
//  //*****// move back d1 > lamda1 > move back d2 > lamda2 & deltalamda //*****//
//  
//  walkB(STEP_B1);
//
//  Serial.println(); // d1
//  Serial.print("dF1: ");  Serial.println(DIST_F1); 
//  Serial.print("dD1: ");  Serial.println(DIST_D1);
//
//  Serial.println();  //lamda1
//  motorValF = analogRead(A1);
//  Serial.print("sF1: ");  Serial.println(analogRead(A1));
//  digitalWrite(6, HIGH);
//  delay(DELAY_S);
//  motorValDR = analogRead(A3);
//  Serial.print("sDR1: ");  Serial.println(analogRead(A3));
//  digitalWrite(6, LOW);
//  delay(DELAY_S);
//  motorValDL = analogRead(A3);
//  Serial.print("sDL1: ");  Serial.println(analogRead(A3));
//  
//  walkB(STEP_B2);
//
//  Serial.println();  // d2
//  Serial.print("dF2: ");  Serial.println(DIST_F2);
//  Serial.print("dD2: ");  Serial.println(DIST_D2);
//
//  Serial.println(); // lamda2
//  motorValF = analogRead(A1) - motorValF;
//  Serial.print("sF2: ");  Serial.println(analogRead(A1)); 
//  digitalWrite(6, HIGH);
//  delay(DELAY_S);
//  motorValDR = analogRead(A3) - motorValDR;
//  Serial.print("sDR2: ");  Serial.println(analogRead(A3));
//  digitalWrite(6, LOW);
//  delay(DELAY_S);
//  motorValDL = analogRead(A3) - motorValDL;
//  Serial.print("sDL2: ");  Serial.println(analogRead(A3));
//
//  Serial.println(); // deltalamda = lamda2 - lamda1
//  Serial.print("sFd: ");  Serial.println(motorValF);
//  Serial.print("sDRd: ");  Serial.println(motorValDR);
//  Serial.print("sDLd: ");  Serial.println(motorValDL);
//
//  //*****// rotate right > lamdaR1 > rotate left > lamdaL1 > rotate right > lamda2/2 & deltalamda //*****//
//  Serial.println(); // d1
//  Serial.print("dR1: ");  Serial.println(DIST_S1);
//  Serial.print("dL1: ");  Serial.println(DIST_S1);
//
//  Serial.println();  //lamda1
//  rotateR(STEP_S);
//  delay(DELAY_T);
//  motorValR = analogRead(A0);
//  Serial.print("sR1: ");  Serial.println(analogRead(A0)); 
//  rotateL(2*STEP_S);
//  delay(DELAY_T); 
//  motorValL = analogRead(A2);
//  Serial.print("sL1: ");  Serial.println(analogRead(A2)); 
//  rotateR(STEP_S);
//  delay(DELAY_T);
//
//  Serial.println(); // d2
//  Serial.print("dR2: ");  Serial.println(DIST_S2);
//  Serial.print("dL2: ");  Serial.println(DIST_S2);
//
//  Serial.println();  //lamda2
//  motorValR = analogRead(A0) - motorValR;
//  Serial.print("sR2: ");  Serial.println(analogRead(A0));
//  motorValL = analogRead(A2) - motorValL;
//  Serial.print("sL2: ");  Serial.println(analogRead(A2));
//
//  Serial.println(); // deltalamda = lamda2 - lamda1
//  Serial.print("sRd: ");  Serial.println(motorValR); 
//  Serial.print("sLd: ");  Serial.println(motorValL);
//
//  //*****// calculate k > calculate (deltalamda'+lamdao) or wallVals/motorVals //*****//
//
//  long KF = pow(DIST_F2-DIST_F1,2) * -motorValF;
//  long KR = pow(DIST_S2-DIST_S1,2) * -motorValR;
//  long KL = pow(DIST_S2-DIST_S1,2) * -motorValL;
//  long KDR = pow(DIST_D2-DIST_D1,2) * -motorValDR;
//  long KDL = pow(DIST_D2-DIST_D1,2) * -motorValDL;
//
//  Serial.println(); // k = (d2 - d1)^2 * deltalamda = deltad^2 * deltalamda
//  Serial.print("KF: ");  Serial.println(KF); 
//  Serial.print("KR: ");  Serial.println(KR);
//  Serial.print("KL: ");  Serial.println(KL);
//  Serial.print("KDR: ");  Serial.println(KDR);
//  Serial.print("KDL: ");  Serial.println(KDL);
//
//  wallValF = max(0, min(1026, KF / (pow(WALL_F, 2))));
//  wallValR = max(0, min(1026, KR / (pow(WALL_S, 2))));
//  wallValL = max(0, min(1026, KL / (pow(WALL_S, 2))));
//  motorValF = max(0, min(1026, KF / (pow(MOTOR_F, 2))));
//  motorValR = max(0, min(1026, KR / (pow(MOTOR_S, 2))));
//  motorValL = max(0, min(1026, KL / (pow(MOTOR_S, 2))));
//  motorValDR = max(0, min(1026, KDR / (pow(MOTOR_D, 2))));
//  motorValDL = max(0, min(1026, KDL / (pow(MOTOR_D, 2))));
//
//  Serial.println(); // lamda = k / d^2
//  Serial.print("WF: " ); Serial.println(wallValF);
//  Serial.print("WR: " ); Serial.println(wallValR);
//  Serial.print("WL: " ); Serial.println(wallValL);
//  Serial.print("MF: " ); Serial.println(motorValF);
//  Serial.print("MR: " ); Serial.println(motorValR);
//  Serial.print("ML: " ); Serial.println(motorValL);
//  Serial.print("MDR: " ); Serial.println(motorValDR);
//  Serial.print("MDL: " ); Serial.println(motorValDL);
//  
//  //*****// center forward > turn 180* //*****//
//  
//  delay(1000);
//  
//  centerF();
//  turnR(dir); turnR(dir);
//} // end calibrateSensors


// Senses wall while holding still and changes the mouse's maze. Should be ran in every unexplored cell.
void senseWall(char &mou, byte dir, byte row, byte col){
  short wallAveL = 0;
  short wallAveF = 0;
  short wallAveR = 0;
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
