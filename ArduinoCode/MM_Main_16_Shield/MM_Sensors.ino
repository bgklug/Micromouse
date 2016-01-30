#define WALLMARG_L   100  // Value subtracted from the value required for left sensor to confirm a wall's existence.
#define WALLMARG_F   50  // Value subtracted from the value required for forward sensor to confirm a wall's existence.
#define WALLMARG_R   100  // Value subtracted from the value required for right sensor to confirm a wall's existence.
#define MOTORMARG_L  50  // Value added to the value required for the left sensor to tell the left motor to turn away from a wall.
#define MOTORMARG_F  25  // Value added to the value required for the front sensor to tell both motors stop.
#define MOTORMARG_R  50  // Value added to the value required for the right sensor to tell the right motor to turn away from a wall.
#define MOTORMARG_D  50  // Value added to the value required for the diagonal sensors to tell the motors to turn away from walls (and pegs).
#define AVECOUNT     3   // Number of readings to average together (for senseWall).
#define DELAY_S      5   // Delay in milliseconds between each wall sensor reading (during senseWall).
#define DELAY_C      0   // Delay in milliseconds between centering and anything else.

int centerUL;
int centerUR;
int wallValL;
int wallValF;
int wallValR;
int motorValL;
int motorValF;
int motorValR;
int motorValD;


// Calibrates global sensor values.
void calibrateSensors () {
  wallValL = analogRead(A2) - WALLMARG_L;
  wallValF = analogRead(A1) - WALLMARG_F;
  wallValR = analogRead(A0) - WALLMARG_R;
  motorValL = analogRead(A2) + MOTORMARG_L;
  motorValF = analogRead(A1) - MOTORMARG_F;
  motorValR = analogRead(A0) + MOTORMARG_R;
  motorValD = analogRead(A3) + MOTORMARG_D;
  turnR(dir); turnR(dir);
} // end calibrateSensors


// Senses wall while holding still and changes the mouse's maze. Should be ran in every unexplored cell.
void senseWall(char mou[SIZE][SIZE], byte dir, byte row, byte col){
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
  wallChange(mou[row][col], (dir + 3)%4, (wallAveL > wallValL)); // Left-Side
  wallChange(mou[row][col], dir, (wallAveF > wallValF));         // Forward
  wallChange(mou[row][col], (dir + 1)%4, (wallAveR > wallValR)); // Right-Side
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
  if(analogRead(A2) > motorValL){ motorL->step(1, FORWARD, SINGLE); return true; }               // Left-Side
  if(analogRead(A1) > motorValF){ return true; }                                                 // Forward
  if(analogRead(A0) > motorValR){ motorR->step(1, FORWARD, SINGLE); return true; }               // Right-Side
  if((analogRead(A3) > motorValD) && select){ motorL->step(1, FORWARD, SINGLE); return true; }   // Left-Diagonal
  if((analogRead(A3) > motorValD) && !select){ motorR->step(1, FORWARD, SINGLE); return true; }  // Right-Diagonal
  /*
  Serial.println("senseMotor:");
  Serial.print("L: ");  Serial.println(analogRead(A2));  //Left
  Serial.print("F: ");  Serial.println(analogRead(A1));  //Forward
  Serial.print("R: ");  Serial.println(analogRead(A0));  //Right
  Serial.print("D: ");  Serial.println(analogRead(A3));  //Diagonal
  */
  return false;
} // end senseMotor


void centerF(){
  while (analogRead(A1) < motorValF){
    motorL->step(1, FORWARD, SINGLE);
    motorR->step(1, FORWARD, SINGLE); 
    senseMotor();
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
    motorL->step(1, FORWARD, SINGLE);
    motorR->step(1, BACKWARD, SINGLE); 
  }
  while (analogRead(A2)<centerUL && analogRead(A0)>centerUR){
    motorL->step(1, BACKWARD, SINGLE);
    motorR->step(1, FORWARD, SINGLE);
  }
} // end centerUpost
