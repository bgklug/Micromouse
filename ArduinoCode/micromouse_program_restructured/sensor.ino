/*
 * Sensor reading 
 */
unsigned char sensorPin[5] = {0, 1, 2, 3, 4};  // {right, rightdiag, center, leftdiag, left}
//int sensorValue[5];
long offset[5];

void sensorSetup() {
  int iterations = 1000;
  unsigned char time = 2;  // seconds
  for (unsigned char i = 0; i < 5; i++) {  // reset offset values
    offset[i] = 0;
  }
  
  for (int i = 0; i < iterations; i++) {  // make 'iteration' number of readings
    for (unsigned char i = 0; i < 5; i++) {  // add up all the readings over the defined time
      offset[i] += analogRead(sensorPin[i]);
    }
    delay(time*1000/iterations);
  }
  for (unsigned char i = 0; i < 5; i++) {  // divide by the number of iterations to get the average
    offset[i] /= iterations;
  }
  
  for (unsigned char i = 0; i < 5; i++) {  // print all offsets
    Serial.println(offset[i]);
  }
}

boolean sensorRead(unsigned char side) {
  return analogRead(sensorPin[side]) > 100;  // will return 1 for wall or 0 for space
}

void senseWall(unsigned char dir, unsigned char row, unsigned char col) {
  wallChange(m[row][col], (dir + 3)%4, sensorRead(4));  // sensorRead LEFT
  wallChange(m[row][col], dir, sensorRead(2));          // sensorRead FORWARD
  wallChange(m[row][col], (dir + 1)%4, sensorRead(0));  // sensorRead RIGHT
}
