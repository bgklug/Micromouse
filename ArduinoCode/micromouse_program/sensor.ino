byte sensorPin[5] = {0, 1, 2, 3, 4};  // right, rightdiag, center, leftdiag, left
int sensorValue[5];
long offset[5];

int threshold = -200;

void sensorSetup() {
  int iterations = 1000;
  byte time = 2;  // seconds
  for (byte i = 0; i < 5; i++) {  // reset offset values
    offset[i] = 0;
  }
  
  for (int i = 0; i < iterations; i++) {  // make 'iteration' number of readings
    for (byte i = 0; i < 5; i++) {  // add up all the readings over the defined time
      offset[i] += analogRead(sensorPin[i]);
    }
    delay(time*1000/iterations);
  }
  for (byte i = 0; i < 5; i++) {  // divide by the number of iterations to get the average
    offset[i] /= iterations;
  }
  
  for (byte i = 0; i < 5; i++) {  // print all offsets
    Serial.println(offset[i]);
  }
}

boolean sensorRead(byte side) {
//  Serial.print("(");
//  Serial.print(analogRead(sensorPin[side]));
//  Serial.print(")");
//  Serial.println((analogRead(sensorPin[side]) - offset[side]) > threshold);
//  return (analogRead(sensorPin[side]) - offset[side]) > threshold;  // will return 1 for wall or 0 for space
  return analogRead(sensorPin[side]) > 100;  // will return 1 for wall or 0 for space
}

void senseWall(byte dir, byte row, byte col, char m[SIZE][SIZE][5]) {
  m[row][col][(dir + 3)%4] = sensorRead(4);  // sensorRead LEFT
  m[row][col][dir] = sensorRead(2);          // sensorRead FORWARD
  m[row][col][(dir + 1)%4] = sensorRead(0);  // sensorRead RIGHT
}
