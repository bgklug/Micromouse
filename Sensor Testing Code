/*
  AnalogReadSerial
 Reads an analog input on pin 0, prints the result to the serial monitor.
 Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
 
 This example code is in the public domain.
 */

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int e = readAvgInput(A0);
  int n = readAvgInput(A1);
  int w = readAvgInput(A2);
  // print out the value you read:
  Serial.print(F("East"));
  Serial.print("\t");
  Serial.println(e);
  Serial.print(F("North"));
  Serial.print("\t");
  Serial.println(n);
  Serial.print(F("West"));
  Serial.print("\t");
  Serial.println(w);
  Serial.println("");
  delay(2500);        // delay in between reads for stability
} 
int readAvgInput(int pin) {
  float sum = 0;

  for (int i=0; i < 3; i++) {
    sum += analogRead(pin);
  }
  return sum/3;
}

