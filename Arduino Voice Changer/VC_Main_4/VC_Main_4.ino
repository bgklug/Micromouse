// NOTES:
// left this project for a month or so... figuring out how well it functions...
// It doesn't work in its current state...
// Right now: the ADC is triggered in a timer interrupt
// Change to: the timer is used as a trigger source for the ADC directly (in ADCSRB)

//#include <Wire.h>
#include <Adafruit_MCP4725.h>
//#include <TimerOne.h>

Adafruit_MCP4725 dac;

#define BIT0 0
#define BIT1 1
#define BIT2 2
#define BIT3 3
#define BIT4 4
#define BIT5 5
#define BIT6 6
#define BIT7 7

// set both equal to 1 for unchanged voice.
#define CHUNK 512 // max ~ 850 from dynamic memory, max = 1023 from uint16_t voiceIn/voiceOut.
#define FSEL_INCON (16 / 1023 / 2) // used to convert analogRead input into one of the levels in fSelCase.
#define FSEL_PIN 1 // input fSelPin = A1
#define FREF_PIN 13 // output 5v reference, to be voltaged divided with 10 kOhm resistor and POT.
#define FSEL_TIME (unsigned long)1024 // used in if statement to modulate millis(), to regulate analogRead for fSelCase.
#define COUNTER_MUX_MAX 10000

boolean fSelReady = true;
uint8_t fSelCase = 0; // percent speed to play back voice (  15 |  14 |  13 |  12 |  11 |  10 |   9 |   8 |   0 |  1   |  2   |  3  |  4   |  5   |  6   |  7
                      //                                 =  xxx | 400 | 300 | 250 | 200 | 175 | 150 | 125 | 100 | 87.5 | 75.0 |67.5 | 50.0 | 37.5 | 25.0 | 12.5).

uint16_t voiceIn = 1; //ahead by 1 to prevent permanent simultaneous read/write when modSlow = 1.
uint16_t voiceOut = 0;
int speakerOut = 11;
uint16_t voiceVec[CHUNK];
int counterMUX = 1;

void setup()
{
  // ---------- DAC ----------//
  dac.begin(0x62);
  
  // ---------- Clock ---------- //
  // CLKPR - Clock Prescale Register
  // CLKPCE: Clock Prescaler Change Enable
  // CLKPS[3:0]: Clock Prescaler Select Bits 3-0
  CLKPR = (1 << CLKPCE); // CLKPCE = 1b, CLKPS[3:0] = 0000b, Mandatory to chang CLKPS bits
//  CLKPR |= (1 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0); // 256 prescaler for   31.25 kHz
//  CLKPR |= (0 << CLKPS3) | (1 << CLKPS2) | (1 << CLKPS1) | (1 << CLKPS0); // 128 prescaler for   62.50 kHz
//  CLKPR |= (0 << CLKPS3) | (1 << CLKPS2) | (1 << CLKPS1) | (0 << CLKPS0); //  64 prescaler for  125.00 kHz
//  CLKPR |= (0 << CLKPS3) | (1 << CLKPS2) | (0 << CLKPS1) | (1 << CLKPS0); //  32 prescaler for  250.00 kHz
//  CLKPR |= (0 << CLKPS3) | (1 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0); //  16 prescaler for  500.00 kHz
//  CLKPR |= (0 << CLKPS3) | (0 << CLKPS2) | (1 << CLKPS1) | (1 << CLKPS0); //   8 prescaler for 1000.00 kHz (DEFAULT)
//  CLKPR |= (0 << CLKPS3) | (0 << CLKPS2) | (1 << CLKPS1) | (0 << CLKPS0); //   4 prescaler for 2000.00 kHz
//  CLKPR |= (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (1 << CLKPS0); //   2 prescaler for 4000.00 kHz
  CLKPR |= (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0); //   1 prescaler for 8000.00 kHz
//  CLKPR &= ~(1 << CLKPCE);

  // ---------- ADC10 ---------- //
  // http://www.robotplatform.com/knowledge/ADC/adc_tutorial_3.html
  // ADMUX - ADC Multiplexer Selection Register
  // REFS[1:0]: Reference Selection Bits 1-0
  // ADLAR: ADC Left Adjust Result
  // MUX[3:0]: Analog Channel Selection Bits 3-0
  ADMUX |= (0 << REFS1) | (1 << REFS0);  // AVcc with external capacitor at AREF pin.
  ADMUX |= (0 & 0x0F);    // ADC channel select ADC0.
  // ADCSRA - ADC Control and Status Register A
  // ADEN: ADC Enable
  // ADSC: ADC Start Conversion
  // ADATE: ADC Auto Trigger Enable
  // ADIF: ADC Interrupt Flag
  // ADIE: ADC Interrupt Enable
  // ADPS[2:0]: ADC Prescaler Select Bits 2-0
  ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADIE); // ADC enabled, ADC auto trigger enabled, ADC interrupt enabled.
                                                          // Prescaler | 8000 kHz | 13 Cycles | 13.5 Cycles
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
  // ADCSRB - ADC Control and Status Regiter B
  // ACME: Analog Comparator Multiplexer Enable
  // ADTS[2:0]: ADC Auto Trigger Source
//  ADCSRB |= (0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0); // Free Running Mode
  ADCSRB |= (0 << ADTS2) | (1 << ADTS1) | (1 << ADTS0); // Timer/Counter0 Compare Match A
//  ADCSRB |= (1 << ADTS2) | (0 << ADTS1) | (0 << ADTS0); // Timer/Counter0 Overflow

  // ---------- Timer0 ---------- //
  // TCCR0A - Timer/Counter Control Register A
  // COM0A[1:0]: Compare Match Output A Mode Bits 1-0
  // COM0B[1:0}: Compare Match Output B Mode Bits 1-0
  // WGM0[1:0]: Waveform Generation Mode
//  TCCR0A |= (0 << COM0A1) | (0 << COM0A0); // Normal port operation, OC0A disconnected.
  TCCR0A |= (0 << COM0A1) | (1 << COM0A0); // Toggle OC0A on Compare Match.
  TCCR0A |= (0 << COM0B1) | (0 << COM0B0); // Normal port operation, OC0B disconnected.
//  TCCR0A |= (0 << COM0B1) | (1 << COM0B0); // Toggle OC0B on Compare Match.
//  TCCR0A |= (0 << WGM01) | (0 << WGM00); // Normal operation, overflow.
  TCCR0A |= (1 << WGM01) | (0 << WGM00); // Clear Timer on Compare Match (CTC) operation.
  // TCCR0B - Timer/Counter Control Register B
  // FOC0A: Force Output Compare A
  // FOC0B: Force Output Compare B
  // WGM0[2]: Waveform Generation Mode
  // CS0[2:0]: Clock Select Bits 2-0
//  TCCR0B |= (0 << CS02) | (0 << CS01) | (0 << CS00); // No clock source
//  TCCR0B |= (0 << CS02) | (0 << CS01) | (1 << CS00); // N = 1
  TCCR0B |= (0 << CS02) | (1 << CS01) | (0 << CS00); // N = 8
//  TCCR0B |= (0 << CS02) | (1 << CS01) | (1 << CS00); // N = 64
//  TCCR0B |= (1 << CS02) | (0 << CS01) | (0 << CS00); // N = 256
//  TCCR0B |= (1 << CS02) | (0 << CS01) | (1 << CS00); // N = 1024
// TCNT0 - Timer/Counter Register
  // f,overflow = clk / [TOP * N] = 8000000 / [0xFF * 8] = 3921.56862745098 Hz
  // OCR0A - Output Compare Register A
  OCR0A = 51; // f,Aout = clk / [2 * N * (1 + OCR0A)] = 8000000 / [2 * 8 * (1 + 51) = 9615.384615384615 Hz (increase OCR0A if problems arise)
  // OCR0B - Output Compare Register B
//  OCR0B = 0;
  // TIMSK0 - Timer/Counter Interrupt Mask Register
  // OCIE0B: Timer/Counter Compare Match B Interrupt Enable
  // OCIE0A: Timer/Counter Compare Match A Interrupt Enable
  // TOIE0: Timer/Counter Overflow Interrupt Enable
  TIMSK0 = (1 << OCIE0A); // Timer0 Compare Match A Interrupt enabled.
  //GTCCR - General Timer/Counter Control Register
//  GTCCR |= (1 << PSRSYNC);

  // ---------- LPM ----------//
  // SMCR - Sleep Mode Control Register
  // SM[2:0]: Sleep Mode Select Bits 2-0
  // SE: Sleep Enable
//  SMCR |= (0 << SM2) | (0 << SM1) | (0 << SM0); // Idle
//  SMCR |= (0 << SM2) | (0 << SM1) | (1 << SM0); // ADC Noise Reduction
//  SMCR |= (0 << SM2) | (1 << SM1) | (0 << SM0); // Power-down
//  SMCR |= (0 << SM2) | (1 << SM1) | (1 << SM0); // Power-save
//  SMCR |= (1 << SM2) | (0 << SM1) | (0 << SM0); // Reserved
//  SMCR |= (1 << SM2) | (0 << SM1) | (1 << SM0); // Reserved
//  SMCR |= (1 << SM2) | (1 << SM1) | (0 << SM0); // Standby
//  SMCR |= (1 << SM2) | (1 << SM1) | (1 << SM0); // External Standby

  // ---------- I/O Pins ---------- //
//  pinMode(A1, INPUT);
//  pinMode(FREF_PIN, OUTPUT);
//  digitalWrite(FREF_PIN, HIGH); // to be used in voltage divider circuit for fSelCase.

  // ---------- Final Setup ---------- //
  // SREG - AVR Status Register
  // BIT7 (I): Global Interrupt Enable
  SREG = (1 << BIT7);
}

// NOT NEEDED NOW THAT I CHANGED TO USING AUTOTRIGGER ON THE ADC!
//ISR(TIMER2_COMPA_vect)
//{
////  SMCR &= ~(1 << SE); // sleep disabled.
////  dac.setVoltage(voiceVec[voiceOut] << 2, false);
//  ADMUX |= (ADMUX & 0xF0) | (0 & 0x0F); // ADC channel select A0.
//  ADCSRA |= (1 << ADSC); // Start ADC conversion.
//
////  if (count to something)
////  {
////    ADMUX |= (ADMUX & 0xF0) | (0 & 0x0F); // ADC channel select A1.
////    ADCSRA |= (1 << ADSC); // Start ADC conversion.
////  }
//}

ISR(ADC_vect)
{
//  SMCR &= ~(1 << SE); // sleep disabled.
  
  uint8_t low, high;
  low = ADCL;
  high = ADCH;

  uint16_t tempADC = (high << 8) | low;

  if (counterMUX == COUNTER_MUX_MAX)
  {
    // could probably still use this value... but i'll deal with that later...
    ADCSRA |= (0 << ADATE) | (0 << ADIE); // ADC auto trigger disabled, ADC interrupt disnabled.
    ADMUX |= (1 & 0x0F);    // ADC channel select ADC1, read nob value next sample.
    ADCSRA |= (1 << ADATE) | (1 << ADIE); // ADC auto trigger enabled, ADC interrupt enabled.
    counterMUX = 0;
  }
  else if (counterMUX == 0)
  {
    //fSelCase = floor(tempADC / 64); // 1024 / 16 = 64
    ADCSRA |= (0 << ADATE) | (0 << ADIE); // ADC auto trigger disabled, ADC interrupt disnabled.
    ADMUX |= (0 & 0x0F);    // ADC channel select ADC0, start reading voice again.
    ADCSRA |= (1 << ADATE) | (1 << ADIE); // ADC auto trigger enabled, ADC interrupt enabled.
    counterMUX++;
  }
  else {
    voiceVec[voiceIn] = tempADC;
    voiceIn = (voiceIn + 1) % CHUNK;
    switch (fSelCase){
      case 0: // 100%
        voiceOut = (voiceOut + 1) % CHUNK; // 8/8
      break;
      case 1: // 87.5%
        if (voiceIn % 8){ voiceOut = (voiceOut + 1) % CHUNK; } // 7/8
      break;
      case 2: // 75.0%
        if (voiceIn % 4){ voiceOut = (voiceOut + 1) % CHUNK; } // 6/8
      break;
      case 3: // 67.5%
        if (voiceIn % 3){ voiceOut = (voiceOut + 1) % CHUNK; } // 5/8
      break;
      case 4: // 50.0%
        if (voiceIn % 2){ voiceOut = (voiceOut + 1) % CHUNK; } // 4/8
      break;
      case 5: // 37.5%
        if (!(voiceIn % 3)){ voiceOut = (voiceOut + 1) % CHUNK; } // 3/8
      break;
      case 6: // 25.0%
        if (!(voiceIn % 4)){ voiceOut = (voiceOut + 1) % CHUNK; } // 2/8
      break;
      case 7: // 12.5%
        if (!(voiceIn % 8)){ voiceOut = (voiceOut + 1) % CHUNK; } // 1/8
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
      default: break; // do nothing, includes case 15 (analogRead from fSelPin = 1023).
      counterMUX++;
    }
  }
}

void loop(void)
{
  // ---------- LPM  ---------- //
//  SMCR = (1 << SE); // Sleep enabled.
//  sleep_mode();
  dac.setVoltage(voiceVec[voiceOut] << 2, false);
}
