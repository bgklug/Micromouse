#define WALLVAL_L 100    // Value required from sensors to confirm a wall's existence.
#define WALLVAL_F 100    // Value required from sensors to confirm a wall's existence.
#define WALLVAL_R 100    // Value required from sensors to confirm a wall's existence.
#define AVECOUNT 5       // Number of readings to average together (for senseWall).
#define MOTORVAL_S 300   // Value required from side sensors to tell the motors to turn away from a wall.
#define MOTORVAL_F 800   // Value required from front sensor to tell the motors to stop.
#define MOTORVAL_D 150   // Value required from the diagonal sensors to tell the motors to turn away from wall.
#define DELAY_C 100      // Delay in milliseconds between centering and anything else.
#define DELAY_S 100      // Delay in milliseconds between each wall sensor reading (during senseWall).


// Senses wall while holding still and changes the mouse's maze. Should be ran in every unexplored cell.
void senseWall(char mou[SIZE][SIZE], byte dir, byte row, byte col){
  int wallAveL = 0;
  int wallAveF = 0;
  int wallAveR = 0;
  for (int i=0; i<AVECOUNT; i++){
    wallAveL += analogRead(A2);
    wallAveF += analogRead(A1);
    wallAveR += analogRead(A0);
    delay(DELAY_S);
  }
  wallAveL = wallAveL / AVECOUNT;
  wallAveF = wallAveF / AVECOUNT;
  wallAveR = wallAveR / AVECOUNT;
  wallChange(mou[row][col], (dir + 3)%4, (wallAveL > WALLVAL_L)); // Left-Side
  wallChange(mou[row][col], dir, (wallAveF > WALLVAL_F));         // Forward
  wallChange(mou[row][col], (dir + 1)%4, (wallAveR > WALLVAL_R)); // Right-Side
  
  Serial.println("senseWall:");
  Serial.print("L: ");  Serial.println(wallAveL);  //Left
  Serial.print("F: ");  Serial.println(wallAveF);  //Forward
  Serial.print("R: ");  Serial.println(wallAveR);  //Right
  
} // end senseWall


// Senses walls while moving and controlls the motors. Should be ran every step.
// Consider having another function to find highest sensor value, then use a case statement.
boolean senseMotor(){
  if(analogRead(A2) > MOTORVAL_S){ motorL->step(1, FORWARD, SINGLE); return true; }               // Left-Side
  if(analogRead(A1) > MOTORVAL_F){ return true; }                                                 // Forward
  if(analogRead(A0) > MOTORVAL_S){ motorR->step(1, FORWARD, SINGLE); return true; }               // Right-Side
  if((analogRead(A3) > MOTORVAL_D) && select){ motorL->step(1, FORWARD, SINGLE); return true; }   // Left-Diagonal
  if((analogRead(A3) > MOTORVAL_D) && !select){ motorR->step(1, FORWARD, SINGLE); return true; }  // Right-Diagonal
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
  while (analogRead(A1) < MOTORVAL_F){
    motorL->step(1, FORWARD, SINGLE);
    motorR->step(1, FORWARD, SINGLE); 
    senseMotor();
  }
  delay(DELAY_C);
} // end centerF
