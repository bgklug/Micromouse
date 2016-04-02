#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_L3GD20_U.h>

int speedPin = 11;
int m1A = 5;
int m1B = 6;
int m2A = 9;
int m2B = 10;
float baselineGyroZ;
bool first = true;

#define DEBUG true

Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);

void setup()
{
  pinMode(m1A, OUTPUT);
  pinMode(m1B, OUTPUT);
  pinMode(m2A, OUTPUT);
  pinMode(m2B, OUTPUT);
  pinMode(speedPin, OUTPUT);

  if(DEBUG){
    Serial.begin(9600);
  }
  
  gyro.enableAutoRange(true);
  /* Initialise the sensor */
  if(!gyro.begin())
  {
    /* There was a problem detecting the L3GD20 ... check your connections */
   if(DEBUG){ Serial.println("Ooops, no L3GD20 detected ... Check your wiring!");}
    while(1);
  }
  //event.gyro.z
  float sum = 0;
  for(int ii = 0; ii < 10; ++ii){
    sensors_event_t event; 
    gyro.getEvent(&event);
    sum += event.gyro.z;
    delay(5);
  }
  baselineGyroZ = sum/10;
  if(DEBUG){
    Serial.print("Baseline Z = ");Serial.println(baselineGyroZ);
  }
  
  
}

float w;
float wold;
float dw;
int start;
int stop;
 
void loop()
{
  if(first){start = millis();}
  /* Get a new sensor event */ 
  sensors_event_t event; 
  gyro.getEvent(&event);

  if(!first){
    wold =w;
  }
  else{wold = 0;}
  w = event.gyro.z-baselineGyroZ;

  if(!first){start = stop;}
  stop = millis();
  dw = (w - wold)/(stop-start);

  if(DEBUG){Serial.print("W: "); Serial.println(w);}
  int speed = 255/2 ;
  setMotor1(speed-50.0*w, 0);
  setMotor2(speed+50.0*w, 0);
  
 
  if(first)
  {first= false;}
}

 
void setMotor1(int speed, boolean reverse)
{
  analogWrite(speedPin, speed);
  digitalWrite(m1A, reverse);
  digitalWrite(m1B, ! reverse);
}
void setMotor2(int speed, boolean reverse)
{
  analogWrite(speedPin, speed);
  digitalWrite(m2A, ! reverse);
  digitalWrite(m2B, reverse);
}



