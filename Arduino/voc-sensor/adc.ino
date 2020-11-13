#define SAMPLES 64  // Oversampling for ADC

unsigned int temp, temp_avg = 0;

unsigned int volt[3];

void initialiseADC() {
  // Set up internal reference 1.1V
  analogReference(INTERNAL);
}

void adcValues() {
  // Reference maths for mV is: 1 / ((1024 / 1100) * (499 / (10000 + 499)));
  // which is 22.601
  unsigned int avolt[2] = {0, 0};
  int i;
  for (i = 0; i < SAMPLES; ++i) {
    avolt[0] += analogRead(VOLT_A);
  }  
  avolt[0] = avolt[0] / SAMPLES;
  
  for (i = 0; i < SAMPLES; ++i) {
    avolt[1] += analogRead(VOLT_B);
  }
  avolt[1] = avolt[1] / SAMPLES;

  volt[0] = avolt[0] * 45 / 2;
  volt[1] = avolt[1] * 45 / 2;
  volt[2] = volt[1] - volt[0];
}
