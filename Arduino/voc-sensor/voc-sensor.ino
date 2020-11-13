
#include <Wire.h>

#define DEBUG_NO

// Pin board definitions
#define OLED_ENABLE 2
#define VOC_WAKE 4
#define VOLT_A A3
#define VOLT_B A2
#define WHEEL A7
#define MODE 8
#define OUT_A 6
#define OUT_B 10
#define OUT_C 9
#define OUT_D 11
#define BUTT_AC 13
#define BUTT_BD 7

void setup() {
  initialiseADC();
  Serial.begin(9600);
  Serial.println(F("Initialising..!"));
  initialiseDisplay();
  initialiseVOC();
  initialiseOutputs();
}

void loop() {
  if (readVOC()) {
    adcValues();
    incrementDataCounter();
  }
  lowPowerMode();
  displayLoopStart();
  readButtons();
  if (menuMode() == 0) {
    graphValues();
    displayValues();
  }
  if (menuMode() == 1) {
    setOutputs(0, wheelDraw(readOutputs(0)));
  }
  displayLoopEnd();
}
