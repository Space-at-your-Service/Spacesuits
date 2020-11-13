#include "Adafruit_CCS811.h"
Adafruit_CCS811 ccs;

int readVOC() {
  // Can be used as a blocking function when returning int
  if (ccs.available()) {
    if (!ccs.readData()) {
      co2[data] = ccs.geteCO2();
      if (co2[data] == 0) {
        co2[data] = co2[data - 1];  // Just fluff any numbers during sensor initialisation
        return 0;
      }
      tvoc[data] = ccs.getTVOC();
      return 1;
    }
  }
  return 0;
}

void lowPowerSensor(byte onOrOff) {
  if (onOrOff) {
    ccs.setDriveMode(CCS811_DRIVE_MODE_1SEC); // To switch sensor to 1 SEC
  } else {
    ccs.setDriveMode(CCS811_DRIVE_MODE_10SEC); // To switch sensor to 10 SEC
  }
}

void initialiseVOC() {
  pinMode(VOC_WAKE, OUTPUT);
  digitalWrite(VOC_WAKE, LOW);
  if (!ccs.begin()) {
    Serial.println("Failed to start sensor! Please check your wiring.");
  }

  // Wait for the sensor to be ready
  int i = 0;
  while (!ccs.available()) {
    loadingBars(i);
    display.display();
    i += 2;
  }
}
