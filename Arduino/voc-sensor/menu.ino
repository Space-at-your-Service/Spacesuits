
#include <LowPower.h>

#define TIMEOUT 120000 // Power management timeout in milliseconds
#define MENU 8  // Number of menu items
#define RESOLUTION 128 // Resolution of scrolling

int wheel;
byte powerManagement = 1;
byte button[] = {0, 0, 0, 0, 0};
unsigned long currentMillis = 0;

int readScrollWheel() {
  // Sample and process wheel position
  unsigned int wheel_diff = 0;
  int i;
  for (i = 0; i < SAMPLES; ++i) {
    wheel_diff += analogRead(WHEEL);
  }
  wheel_diff = wheel_diff / SAMPLES;
  if (abs(wheel - wheel_diff) > (1024 / RESOLUTION)) {
    wheel = wheel_diff;
    resetSleepTimer(); // Wake up device if scrolled
  }
  if (wheel < 12) {
    wheel = 0;
  }
  if (wheel > 1012) {
    wheel = 1023;
  }
  return wheel;
}

int wheelScroll() {
  // Scroll the list snapping to intervals of MENU with acceleration notching animation
  int vert_coord = (readScrollWheel() / (1024 / (MENU * 8))) - (MENU * 8 - 1);
  return vert_coord;
}

void readButtons() {
  // Read button presses and display highlighted at bottom of screen
  pinMode( BUTT_AC, INPUT);
  pinMode( BUTT_BD, INPUT);
  delay(1);  // For some reason this is required otherwise button 2 is stuck ON

  button[0] = digitalRead(BUTT_AC);
  button[1] = digitalRead(BUTT_BD);

  pinMode( BUTT_AC, INPUT_PULLUP);
  pinMode( BUTT_BD, INPUT_PULLUP);

  button[2] = !digitalRead(BUTT_AC);
  button[3] = !digitalRead(BUTT_BD);

  if (!digitalRead(MODE)) {
    button[4] = !button[4];
    delay(200);
    resetSleepTimer();
  }

  byte i;
  for (i = 0; i < 4; ++i) {
    if (button[i]) {
      resetSleepTimer();
    }
  }
  buttonDraw(button, 4);
}

byte menuMode() {
  return button[4];
}

void resetSleepTimer() {
  if (!powerManagement) {
    powerManagement = 1;
    lowPowerDisplay(powerManagement);
    lowPowerSensor(powerManagement);
  }
  currentMillis = millis();
}

void lowPowerMode() {
  if (powerManagement) {
    if (millis() - currentMillis > TIMEOUT) {
      currentMillis = millis();
      powerManagement = 0;
      lowPowerDisplay(powerManagement);
      lowPowerSensor(powerManagement);
    }
  } else {
    LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
  }
}
