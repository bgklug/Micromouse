#define chunk 850
#define referenceHigh 200
#define referenceLow  100
#define margin 25
#define multSlow 2

int voiceIn = 0;
int voiceOut = 0;
int sampleCount = 0;
long t, t0;
int speakerOut = 11;
uint16_t voiceVec[chunk];
uint16_t voicemax;

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
  //ADCSRA |= (1 << ADPS2) | (1 << ADPS0);    // 32 prescaler for 38.5 KHz
  ADCSRA |= (1 << ADPS2);                     // 16 prescaler for 76.9 KHz
  //ADCSRA |= (1 << ADPS1) | (1 << ADPS0);    // 8 prescaler for 153.8 KHz

  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements
}

ISR(ADC_vect)
{
  uint8_t low, high;
  low = ADCL;
  high = ADCH;
  voiceVec[voiceIn] = (high << 8) | low;
  voiceIn = (voiceIn + 1) % chunk;

  uint16_t store = voiceVec[voiceOut];
  voiceOut = (voiceOut + (voiceIn % multSlow)) % chunk;
  //if ((voiceVec[voiceOut] > store) && (voiceVec[voiceOut] > referenceHigh)){digitalWrite(speakerOut, HIGH);}
  //else if ((voiceVec[voiceOut] < store) && (voiceVec[voiceOut] > referenceLow)){digitalWrite(speakerOut, LOW);}
  if (voiceVec[voiceOut] > (store + margin)){digitalWrite(speakerOut, HIGH);}
  else if (voiceVec[voiceOut] < (store - margin)){digitalWrite(speakerOut, LOW);}


  //analogWrite(speakerOut, (int)voiceVec[voiceOut]*255/1023);
  voicemax = max(voicemax,voiceVec[voiceIn]);
  
  //if(voiceVec[voiceOut] > referenceHigh){digitalWrite(speakerOut, HIGH);}
  //else if (voiceVec[voiceOut] > referenceLow){digitalWrite(speakerOut, LOW);}

  
  
  sampleCount++;
}

void loop()
{
  t0 = micros();
  sampleCount = 0;
  delay(1000);
  t = t0 - micros();  // calculate elapsed time

  Serial.print("Sampling frequency: ");
  Serial.print((float)sampleCount*1000/t);
  Serial.println(" KHz");

  Serial.print("voicemax: ");
  Serial.println(voicemax);
  voicemax = 0;
  
  /*
  if (numSamples >= 1000)
  {
    t = micros()-t0;  // calculate elapsed time

    Serial.print("Sampling frequency: ");
    Serial.print((float)1000000/t);
    Serial.println(" KHz");
    delay(2000);
    
    // restart
    t0 = micros();
    numSamples=0;
  }
  */
}
