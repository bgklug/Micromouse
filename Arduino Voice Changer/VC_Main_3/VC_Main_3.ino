#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <TimerOne.h>

Adafruit_MCP4725 dac;

// set both equal to 1 for unchanged voice.
#define chunk 512 //max~850 from memory, max=1023 from uint26_t voiceIn/voiceOut.
#define perCase 0 //percent speed to play back voice (0  /1   /2   /3   /4   /5   /6   /7
                   //                               = 100/87.5/75.0/67.5/50.0/37.5/25.0/12.5).

uint16_t voiceIn = 1; //ahead by 1 to prevent permanent simultaneous read/write when modSlow = 1.
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
  //ADCSRA |= (1 << ADPS2) | (1 << ADPS1);    // 64 prescaler for 19.3 kHz
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
  
  switch (perCase){
    case 0: // 100%
      voiceOut = (voiceOut + 1) % chunk; // 8/8
    break;
    case 1: // 87.5%
      if (voiceIn % 8){ voiceOut = (voiceOut + 1) % chunk; } // 7/8
    break;
    case 2: // 75.0%
      if (voiceIn % 4){ voiceOut = (voiceOut + 1) % chunk; } // 6/8
    break;
    case 3: // 67.5%
      if (voiceIn % 3){ voiceOut = (voiceOut + 1) % chunk; } // 5/8
    break;
    case 4: // 50.0%
      if (voiceIn % 2){ voiceOut = (voiceOut + 1) % chunk; } // 4/8
    break;
    case 5: // 37.5%
      if (!(voiceIn % 3)){ voiceOut = (voiceOut + 1) % chunk; } // 3/8
    break;
    case 6: // 25.0%
      if (!(voiceIn % 4)){ voiceOut = (voiceOut + 1) % chunk; } // 2/8
    break;
    case 7: // 12.5%
      if (!(voiceIn % 8)){ voiceOut = (voiceOut + 1) % chunk; } // 1/8
    break;
    case -1: // 125%
      if (voiceIn % 4){ voiceOut = (voiceOut + 2) % chunk; } // 10/8
      else { voiceOut = (voiceOut + 1) % chunk; }
    break;
    case -2: // 150%
      if (voiceIn % 2){ voiceOut = (voiceOut + 2) % chunk; } // 12/8
      else { voiceOut = (voiceOut + 1) % chunk; }
    break;
    case -3: // 175%
      if (!(voiceIn % 4)){ voiceOut = (voiceOut + 2) % chunk; } // 14/8
      else { voiceOut = (voiceOut + 1) % chunk; }
    break;
    case -4: // 200%
      voiceOut = (voiceOut + 2) % chunk; // 16/8
    break;
    case -5: // 250%
      if (!(voiceIn % 3)){ voiceOut = (voiceOut + 2) % chunk; } // 20/8
      else { voiceOut = (voiceOut + 1) % chunk; }
    break;
    case -6: // 300%
      voiceOut = (voiceOut + 3) % chunk; // 24/8
    break;
    case -7: // 400%
      voiceOut = (voiceOut + 4) % chunk; // 32/8
    break;
    default:
    // DO NOTHING.
    break;
  }
}

void loop(void)
{  
  dac.setVoltage(voiceVec[voiceOut], false);
}
