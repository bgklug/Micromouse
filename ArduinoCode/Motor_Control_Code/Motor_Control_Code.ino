/* For right motor, spd*-1 is forward. For left motor, spd is forward */

#include <AccelStepper.h>
AccelStepper motorL(4, 12, 11, 10, 9);
AccelStepper motorR(4, 4, 5, 6, 7);
float spd = 1000;
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
  Serial.begin(9600);
  /*Keyboard.begin();  Put this here for entering values into Arduino*/
  Serial.println("Enter number from 1-4");
}
void loop()
{
//  delay(1000);
  forward(spd);
//  delay(100);
//  forward(spd/2);
//  delay(100);
//  forward(spd/2);
//  delay(100);
//  turn_right(spd/2);
//  delay(100);
//  forward(spd/2);
//  delay(100);
//  forward(spd/2);
//  delay(100);
//  turn_right(spd/2);
//  delay(100);
//  
  if (Serial.available()>0)
  {
   Serial.println("Please enter 1 for forward, 2 for backward, 3 for turn right, and 4 for trun left");
   int input = Serial.read();
   switch(input){
    case 49:
     forward(spd);
     Serial.println(1);
     break;
    case 50:
     backward(spd);
     Serial.println(2);
     break;
    case 51:
     turn_right(spd);
     Serial.println(3);
     break;
    case 52:
     turn_left(spd);
     Serial.println(4);
     break;
    default:
     break;
   }
   delay(2);
  }
  //if (Serial.available()>0)
  //{
  //  Serial.println(5);
  //}
  //delay(1000);
  //input = 0;
  //Serial.println(input);
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
