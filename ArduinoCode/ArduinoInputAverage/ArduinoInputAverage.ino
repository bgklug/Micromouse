#define READ_INPUT_SIZE 100

double readAvgInput(int pin) {
  double sum = 0;
  
  for (int i=0; i < READ_INPUT_SIZE; i++) {
    sum += analogRead(pin);
  }
  return sum/READ_INPUT_SIZE;
}
