#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;
#define DAC_RESOLUTION 9

#define chunk 10 //max~850 from memory, max=1023 from uint26_t voiceIn/voiceOut.
#define referenceHigh 200
#define referenceLow  100
#define offsetDC 332 // from MATLAB testing of quiet room signal
#define margin 5
#define multSlow 2
#define pDelay 1000 // ms (seems to get doubled)

uint16_t voiceIn = 0;
uint16_t voiceOut = 0;
long sampleCount = 0;
long t, t0;
int speakerOut = 11;
uint16_t voiceVec[chunk];
uint16_t voicemax = 0;
uint16_t voicemin = 1023;
uint16_t margHigh = offsetDC + margin;
uint16_t margLow = offsetDC - margin;

void setup()
{
  Serial.begin(115200);
  pinMode(speakerOut, OUTPUT);

  ADCSRA = 0;             // clear ADCSRA register
  ADCSRB = 0;             // clear ADCSRB register
  ADMUX |= (0 & 0x07);    // set A0 analog input pin
  ADMUX |= (1 << REFS0);  // set reference voltage
  //ADMUX |= (1 << ADLAR);  // left align ADC value to 8 bits from ADCH register

  // sampling rate is [ADC clock] / [prescaler] / [conversion clock cycles]
  // for Arduino Uno ADC clock is 16 MHz and a conversion takes 13 clock cycles
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 prescaler for 9.7 kHz
  //ADCSRA |= (1 << ADPS2) | (1 << ADPS1);    // 64 prescaler for 19.3 kHz
  //ADCSRA |= (1 << ADPS2) | (1 << ADPS0);    // 32 prescaler for 38.5 KHz
  //ADCSRA |= (1 << ADPS2);                   // 16 prescaler for 76.9 KHz
  //ADCSRA |= (1 << ADPS1) | (1 << ADPS0);    // 8 prescaler for 153.8 KHz

  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements

  dac.begin(0x62);
}

ISR(ADC_vect)
{
  uint8_t low, high;
  low = ADCL;
  high = ADCH;
  voiceVec[voiceIn] = (high << 8) | low;
  voiceIn = (voiceIn + 1) % chunk;

  //uint16_t store = voiceVec[voiceOut];

  voiceOut = voiceIn - 1 % chunk;  // if you want no voice changing.
  //voiceOut = (voiceOut + (voiceIn % multSlow)) % chunk;
  
  //if ((voiceVec[voiceOut] > store) && (voiceVec[voiceOut] > referenceHigh)){digitalWrite(speakerOut, HIGH);}
  //else if ((voiceVec[voiceOut] < store) && (voiceVec[voiceOut] > referenceLow)){digitalWrite(speakerOut, LOW);}
  
  //if (voiceVec[voiceOut] > (store + margin)){digitalWrite(speakerOut, HIGH);}
  //else if (voiceVec[voiceOut] < (store - margin)){digitalWrite(speakerOut, LOW);}

  //if (voiceVec[voiceOut] > margHigh) {digitalWrite(speakerOut, HIGH);}
  //else if (voiceVec[voiceOut] < margLow) {digitalWrite(speakerOut, LOW);}

  dac.setVoltage(pgm_read_word(voiceVec[voiceOut]), false);

  //analogWrite(speakerOut, (int)voiceVec[voiceOut]*255/1023);
  //voicemax = max(voicemax,voiceVec[voiceIn]);
  //voicemin = min(voicemin,voiceVec[voiceIn]);
  
  //if(voiceVec[voiceOut] > referenceHigh){digitalWrite(speakerOut, HIGH);}
  //else if (voiceVec[voiceOut] > referenceLow){digitalWrite(speakerOut, LOW);}

  sampleCount++;
}

void loop()
{
//  t0 = micros();
//  sampleCount = 0;
//  delay(pDelay);
//  t = micros() - t0;  // calculate elapsed time

//  Serial.print("DeltaT: ");
//  Serial.print((float)t/1000000);
//  Serial.println(" s");
//
//  Serial.print("sampleCount: ");
//  Serial.print(sampleCount);
//  Serial.println(" samples");

//  Serial.print("Sampling frequency: ");
//  Serial.println((float)sampleCount*1000/t);
//  Serial.println(" KHz");

//  Serial.print("voicemax: ");
//  Serial.println(voicemax);
//  voicemax = 0;
//  
//  Serial.print("voicemin: ");
//  Serial.println(voicemin);
//  voicemin = 1023;

//  Serial.println(); // gaptext
}
