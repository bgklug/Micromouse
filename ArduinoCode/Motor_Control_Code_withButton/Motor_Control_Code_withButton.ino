/* For right motor, spd*-1 is forward. For left motor, spd is forward */

#include <AccelStepper.h>
AccelStepper motorL(4, 12, 11, 10, 9);
AccelStepper motorR(4, 4, 5, 6, 7);
float spd = 750;
int i=0;
void forward(float);
void backward(float);
void turn_right(float);
void turn_left(float);
int input = 0;
int buttonState = 0;
int buttonPin = 2;

void setup()
{
  motorL.setMaxSpeed(1000.0);
  motorL.setSpeed(spd);
  motorR.setMaxSpeed(1000.0);
  motorR.setSpeed(spd);
  
  pinMode(buttonPin, INPUT);
}
void loop()
{
  buttonState = digitalRead(buttonPin);
  
  if(buttonState == HIGH)
  {
    forward(spd);
  }
}

void forward(float spd)
{
  int i=0;
  motorR.setSpeed(spd*-1);
  motorL.setSpeed(spd);
  while (i<1000)
  {
    
    motorR.runSpeed();
    motorL.runSpeed();
    i++;    
    delay(5);
  }
}

void backward(float spd)
{
  int i=0;
  motorR.setSpeed(spd);
  motorL.setSpeed(spd*-1);
  while (i<135)
  {
    motorR.runSpeed();
    motorL.runSpeed();
    i++;
    delay(5);
  }
}

void turn_right(float spd)
{
  int i=0;
  motorR.setSpeed(spd);
  motorL.setSpeed(spd);
  while(i<92) //alter this value based on the amount of rotation
  {
    motorR.runSpeed();
    motorL.runSpeed();
    i++;
    delay(5);
  }
}

void turn_left(float spd)
{
  int i=0;
  
  motorR.setSpeed(spd*-1);
  motorL.setSpeed(spd*-1);
  while(i<92) //alter this value based on the amount of rotation
  {
    motorR.runSpeed();
    motorL.runSpeed();
    i++;
    delay(5);
  }
}
