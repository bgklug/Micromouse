#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <TimerOne.h>

Adafruit_MCP4725 dac;

#define chunk 100 //max~850 from memory, max=1023 from uint26_t voiceIn/voiceOut.
#define multSlow 2

uint16_t voiceIn = 0;
uint16_t voiceOut = 0;
int speakerOut = 11;
uint16_t voiceVec[chunk];

void setup()
{
  //Serial.begin(115200);


  ADMUX |= (0 & 0x07);    // set A0 analog input pin
  ADMUX |= (1 << REFS0);  // set reference voltage
  //ADMUX |= (1 << ADLAR);  // left align ADC value to 8 bits from ADCH register

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

  Timer1.initialize(100); // timer period (in microseconds)
  Timer1.attachInterrupt(callback); // blinkLED to run at 9000 Hz
  //Serial.begin(9600);

  dac.begin(0x62);
}

void callback()
{
  uint8_t low, high;
  low = ADCL;
  high = ADCH;
  voiceVec[voiceIn] = (high << 8) | low;
  
  voiceIn = (voiceIn + 1) % chunk;
  //voiceOut = (voiceIn - 1) % chunk;  // if you want no voice changing.
  voiceOut = (voiceOut + (voiceIn % multSlow)) % chunk;
}

void loop(void)
{  
  dac.setVoltage(voiceVec[voiceOut], false);
}
