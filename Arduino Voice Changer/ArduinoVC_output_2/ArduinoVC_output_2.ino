#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <TimerOne.h>

#define chunk 500 //max~800 from memory, max=1023 from uint26_t voiceIn/voiceOut.

uint16_t sampleCount = 0;
uint16_t voiceVec[chunk];

void setup()
{
  ADMUX |= (0 & 0x07);    // set A0 analog input pin
  ADMUX |= (1 << REFS0);  // set reference voltage
  
  // sampling rate is [ADC clock] / [prescaler] / [conversion clock cycles]
  // for Arduino Uno ADC clock is 16 MHz and a conversion takes 13 clock cycles
  //ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 prescaler for 9.7 kHz
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1);    // 64 prescaler for 19.3 kHz
  //ADCSRA |= (1 << ADPS2) | (1 << ADPS0);    // 32 prescaler for 38.5 KHz
  //ADCSRA |= (1 << ADPS2);                   // 16 prescaler for 76.9 KHz
  //ADCSRA |= (1 << ADPS1) | (1 << ADPS0);    // 8 prescaler for 153.8 KHz

  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements
  
  Timer1.initialize(100); // timer period (in microseconds, us, x10^-6)
  Timer1.attachInterrupt(callback);
  Serial.begin(250000);
}

void callback()
{
  if (sampleCount < chunk) {

    uint8_t low, high;
    low = ADCL;
    high = ADCH;
    voiceVec[sampleCount] = (high << 8) | low;

    sampleCount++;
  } 
  else 
  {
    Timer1.detachInterrupt();
  }
}

void loop()
{
  delay(1000);
  
  if (sampleCount >= chunk) {
    for(int i=0; i<chunk; i++){
      Serial.println(voiceVec[i]);
    }
  }

  Serial.println("END");
  while(1){}
}
