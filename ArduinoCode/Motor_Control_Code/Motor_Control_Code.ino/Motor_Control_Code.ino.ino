/* For right motor, spd*-1 is forward. For left motor, spd is forward */

#include <AccelStepper.h>
AccelStepper motorL(4, 12, 11, 10, 9);
AccelStepper motorR(4, 4, 5, 6, 7);
float spd = 500;
int i=0;
int counterForward = 0;
int counterTurn = 0;
void forward(float);
void backward(float);
void turn_right(float);
void turn_left(float);
int input = 0;

void setup()
{
  motorL.setMaxSpeed(1000.0);
  motorL.setSpeed(spd);
  motorR.setMaxSpeed(1000.0);
  motorR.setSpeed(spd);

}
void loop()
{
forward(spd/2);
   delay(100);
  }
  //if (Serial.available()>0)
  //{
  //  Serial.println(5);
  //}
  //delay(1000);
  //input = 0;
  //Serial.println(input);


void forward(float spd)
{
  int i=0,j=0; 
  motorR.setSpeed(spd*-1); // Motor directios are reversed right is left, left is right speed is the rate at which steps are taken max 1000
  motorL.setSpeed(spd*-1); 
  if(counterForward >= 4)  // This is a distance correction, each forward call should move 143.25 steps, the four counter correct for this discrepancy
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
  while (i<143)         // 143 steps corresponds to approximatly 1 cell of the maze
  {
    
    motorR.runSpeed();
    motorL.runSpeed();
    i++;    
    delay(7);            // the delay give times for the motors to actually move.
    if (j ==38)          // a correction for the unbalanced motors, to make the bot drive straight, the without this code the bot will list left, because motorR is actually the left motor
    {
      motorR.runSpeed();
      j = 0;             // by taking one extra step of the left motor every 38 steps the bot is corrected to drive straight
      delay(7);
    }
    j++;
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
