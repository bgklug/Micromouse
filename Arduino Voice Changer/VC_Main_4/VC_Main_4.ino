// NOTES:
// left this project for a month or so... figuring out how well it functions...
// It doesn't work in its current state...
// Right now: the ADC is triggered in a timer interrupt
// Change to: the timer is used as a trigger source for the ADC directly (in ADCSRB)

//#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

#define CHUNK 512           // max ~ 850 from dynamic memory, max = 1023 from uint16_t voiceIn/voiceOut.
#define FSEL_PIN 1          // input fSelPin = A1
#define FREF_PIN 13         // output 5v reference, to be voltaged divided with 10 kOhm resistor and POT.
#define COUNTER_MUX_MAX 1024  //

uint8_t fSelCase = 0; // percent speed to play back voice (  15 |  14 |  13 |  12 |  11 |  10 |   9 |   8 |   7 |  6   |  5   |  4  |  3   |  2   |  1   |  0
                      //                                 =  xxx | 400 | 300 | 250 | 200 | 175 | 150 | 125 | 100 | 87.5 | 75.0 |67.5 | 50.0 | 37.5 | 25.0 | 12.5).

uint16_t voiceIn = 10; // ahead by 1 to prevent permanent simultaneous read/write when modSlow = 1.
uint16_t voiceOut = 0;
int speakerOut = 11;
uint16_t voiceVec[CHUNK];
int counterMUX = COUNTER_MUX_MAX;

void setup()
{
  // ---------- DAC ----------//
  dac.begin(0x62);
  
  // ---------- Clock ---------- //
  // CLKPR - Clock Prescale Register
  // CLKPCE: Clock Prescaler Change Enable
  // CLKPS[3:0]: Clock Prescaler Select Bits 3-0
  CLKPR = (1 << CLKPCE); // CLKPCE = 1b, CLKPS[3:0] = 0000b, Mandatory to change CLKPS bits
//  CLKPR |= (1 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0); // 256 prescaler for   31.25 kHz
//  CLKPR |= (0 << CLKPS3) | (1 << CLKPS2) | (1 << CLKPS1) | (1 << CLKPS0); // 128 prescaler for   62.50 kHz
//  CLKPR |= (0 << CLKPS3) | (1 << CLKPS2) | (1 << CLKPS1) | (0 << CLKPS0); //  64 prescaler for  125.00 kHz
//  CLKPR |= (0 << CLKPS3) | (1 << CLKPS2) | (0 << CLKPS1) | (1 << CLKPS0); //  32 prescaler for  250.00 kHz
//  CLKPR |= (0 << CLKPS3) | (1 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0); //  16 prescaler for  500.00 kHz
//  CLKPR |= (0 << CLKPS3) | (0 << CLKPS2) | (1 << CLKPS1) | (1 << CLKPS0); //   8 prescaler for 1000.00 kHz (DEFAULT)
//  CLKPR |= (0 << CLKPS3) | (0 << CLKPS2) | (1 << CLKPS1) | (0 << CLKPS0); //   4 prescaler for 2000.00 kHz
//  CLKPR |= (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (1 << CLKPS0); //   2 prescaler for 4000.00 kHz
  CLKPR |= (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0); //   1 prescaler for 8000.00 kHz

  // ---------- ADC10 ---------- //
  //#ADMUX - ADC Multiplexer Selection Register
  // REFS[1:0]: Reference Selection Bits 1-0
  // ADLAR: ADC Left Adjust Result
  // MUX[3:0]: Analog Channel Selection Bits 3-0
  ADMUX = (0 << REFS1) | (1 << REFS0);  // AVcc with external capacitor at AREF pin.
  ADMUX |= (0 & 0x0F);    // ADC channel select ADC0.
  //#ADCSRA - ADC Control and Status Register A
  // ADEN: ADC Enable
  // ADSC: ADC Start Conversion
  // ADATE: ADC Auto Trigger Enable
  // ADIF: ADC Interrupt Flag
  // ADIE: ADC Interrupt Enable
  // ADPS[2:0]: ADC Prescaler Select Bits 2-0
  ADCSRA = (1 << ADEN) | (0 << ADATE) | (1 << ADIE); // ADC enabled, ADC Auto Trigger disabled, ADC Interrupts enabled.
  //                                                       Prescaler | 8000 kHz | 13 Cycles | 13.5 Cycles
//  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128       |  ~63 kHz | 4.808 kHz | 4.630 kHz
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0); //  64       |  125 kHz | 9.615 kHz | 9.259 kHz
//  ADCSRA |= (1 << ADPS2) | (0 << ADPS1) | (1 << ADPS0); //  32       |  250 kHz | 19.23 kHz | 18.52 kHz
//  ADCSRA |= (1 << ADPS2) | (0 << ADPS1) | (0 << ADPS0); //  16       |  500 kHz | 38.46 kHz | 37.04 kHz
//  ADCSRA |= (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //   8       | 1000 kHz | 76.92 kHz | 74.07 kHz
//  ADCSRA |= (0 << ADPS2) | (1 << ADPS1) | (0 << ADPS0); //   4       | 2000 kHz | 153.8 kHz | 148.1 kHz
//  ADCSRA |= (0 << ADPS2) | (0 << ADPS1) | (1 << ADPS0); //   2       | 4000 kHz | 307.7 kHz | 296.3 kHz
//  ADCSRA |= (0 << ADPS2) | (0 << ADPS1) | (0 << ADPS0); //   1       | 8000 kHz | 615.4 kHz | 592.6 kHz
  // ADC conversion takes 13 clock cycles?
  // CLKadc must remain between 50 and 200 kHz (CLKadc / 13 = 3.846 to 15.385 kHz), according to datasheet?
  //#ADCSRB - ADC Control and Status Regiter B
  // ACME: Analog Comparator Multiplexer Enable
  // ADTS[2:0]: ADC Auto Trigger Source
  ADCSRB = (0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0); // Free Running Mode.

  // ---------- Final Setup ---------- //
  // SREG - AVR Status Register
  // BIT7 (I): Global Interrupt Enable
  SREG = (1 << 7);
  ADCSRA |= (1 << ADSC); //Start ADC conversion.
}

ISR(ADC_vect)
{  
  uint8_t low, high;
  low = ADCL;
  high = ADCH;

  uint16_t tempADC = (high << 8) | low;

  if (counterMUX == COUNTER_MUX_MAX) // change ADC input channel to read knob.
  {
    changeOutput(tempADC);
    //turn pin on high
    ADMUX = (ADMUX & 0xF0) | (1 & 0x0F);    // ADC channel select ADC1, read nob value next sample.
    counterMUX = 0;
  }
  else if (counterMUX == 0)
  {
    float tempADCdivider = tempADC / 64.0f;
    fSelCase = floor(tempADCdivider); // 1024 / 16 = 64
    ADMUX = (ADMUX & 0xF0) | (0 & 0x0F);    // ADC channel select ADC1, read nob value next sample.
    counterMUX++;
  }
  else // read microphone and prepare output.
  {
    changeOutput(tempADC);
    counterMUX++;
  }
  ADCSRA |= (1 << ADSC); // Start next ADC conversion.
}

void loop(void)
{
  dac.setVoltage(voiceVec[voiceOut] << 2, false);
}

void changeOutput (uint16_t tempADC)
{
  voiceVec[voiceIn] = tempADC;
  voiceIn = (voiceIn + 1) % CHUNK;
  switch (fSelCase){
    case 0: // 12.5%
      if (!(voiceIn % 8)){ voiceOut = (voiceOut + 1) % CHUNK; } // 1/8
    break;
    case 1: // 25.0%
      if (!(voiceIn % 4)){ voiceOut = (voiceOut + 1) % CHUNK; } // 2/8
    break;
    case 2: // 37.5%
      if (!(voiceIn % 3)){ voiceOut = (voiceOut + 1) % CHUNK; } // 3/8
    break;
    case 3: // 50.0%
      if (voiceIn % 2){ voiceOut = (voiceOut + 1) % CHUNK; } // 4/8
    break;
    case 4: // 67.5%
      if (voiceIn % 3){ voiceOut = (voiceOut + 1) % CHUNK; } // 5/8
    break;
    case 5: // 75.0%
      if (voiceIn % 4){ voiceOut = (voiceOut + 1) % CHUNK; } // 6/8
    break;
    case 6: // 87.5%
      if (voiceIn % 8){ voiceOut = (voiceOut + 1) % CHUNK; } // 7/8
    break;
    case 7: // 100%
      voiceOut = (voiceOut + 1) % CHUNK; // 8/8
    break;
    case 8: // 125%
      if (voiceIn % 4){ voiceOut = (voiceOut + 2) % CHUNK; } // 10/8
      else { voiceOut = (voiceOut + 1) % CHUNK; }
    break;
    case 9: // 150%
      if (voiceIn % 2){ voiceOut = (voiceOut + 2) % CHUNK; } // 12/8
      else { voiceOut = (voiceOut + 1) % CHUNK; }
    break;
    case 10: // 175%
      if (!(voiceIn % 4)){ voiceOut = (voiceOut + 2) % CHUNK; } // 14/8
      else { voiceOut = (voiceOut + 1) % CHUNK; }
    break;
    case 11: // 200%
      voiceOut = (voiceOut + 2) % CHUNK; // 16/8
    break;
      case 12: // 250%
      if (!(voiceIn % 3)){ voiceOut = (voiceOut + 2) % CHUNK; } // 20/8
      else { voiceOut = (voiceOut + 1) % CHUNK; }
    break;
    case 13: // 300%
      voiceOut = (voiceOut + 3) % CHUNK; // 24/8
    break;
    case 14: // 400%
      voiceOut = (voiceOut + 4) % CHUNK; // 32/8
    break;
    default: break; // do nothing, includes case 15 (for now?)
  }
}

