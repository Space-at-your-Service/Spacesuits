#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void handlePrints(char *valueName, unsigned int valueToPrint, int decimalPlaces = 0) {
  char str[16];
  if (decimalPlaces == 0) {
    sprintf (str, "%s%d", valueName, valueToPrint);
  }
  if (decimalPlaces != 0) {
    int decimalFactor = 1;
    byte i;
    for (i = 0; i < decimalPlaces; ++i) {
      decimalFactor = decimalFactor * 10;
    }
    unsigned int valueToPrFloat = valueToPrint / decimalFactor;
    unsigned int valueDecimal = valueToPrint % decimalFactor;
    unsigned int n = valueDecimal, n1;
    while (n) {   // Divide by 10 until the number is two digits long
      valueDecimal = n1;
      n1 = n;
      n /= 10;
    }
    // Build the string with max 2 digits after character 'decimal point'
    sprintf (str, "%s%d.%d", valueName, valueToPrFloat, valueDecimal);
  }
  display.println(str);
#ifdef DEBUG
  Serial.print(str);
  Serial.print("\t");
#endif
}

void displayValues() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, wheelScroll());
  handlePrints("CO2: ", co2[data - 1]);
  handlePrints("TVOC: ", tvoc[data - 1]);
  handlePrints("V_A: ", volt[0], 3);
  handlePrints("V_B: ", volt[1], 3);
  handlePrints("V_D: ", volt[2]);
  handlePrints("TEMP: ", temp);
  handlePrints("AVG: ", calculateAverage());
  handlePrints("data: ", data - 1);
}

void graphValues() {
  int mx, co2_dat1, co2_dat2, tvoc_dat, scale;
  int i, offset;

  // Find maximum for scaling
  mx = 100;
  for (i = 0; i < DATA_LEN; ++i) {
    if (co2[i] > mx) {
      mx = co2[i];
    }
  }
  scale = 1 + mx / SCREEN_HEIGHT;

  for (i = 0; i < DATA_LEN; ++i) {
    // Rolling memory management
    offset = data - 1;
    if (offset - i < 0) {
      offset += DATA_LEN;
    }

    // Shuffle readings for next line segment
    co2_dat2 = co2_dat1;
    co2_dat1 = co2[offset - i];
    tvoc_dat = tvoc[offset - i];

    // Draw line graph CO2
    display.drawLine(SCREEN_WIDTH - 2 * (i - 1),  SCREEN_HEIGHT - co2_dat2 / scale,
                     SCREEN_WIDTH - 2 * i,        SCREEN_HEIGHT - co2_dat1 / scale,
                     SSD1306_WHITE);

    if (tvoc_dat > 0) {
      // Draw histogram TVOC
      display.drawLine(SCREEN_WIDTH - 2 * i,        SCREEN_HEIGHT - 2,
                       SCREEN_WIDTH - 2 * i,        SCREEN_HEIGHT - 2 - tvoc_dat / (scale / 3),
                       SSD1306_WHITE);
    }

    // Draw scatter avg
    display.drawPixel(1 + SCREEN_WIDTH - 2 * i,   SCREEN_HEIGHT - avg[i] / scale,
                      SSD1306_WHITE);
  }
}

void displayLoopStart() {
  display.clearDisplay();
}

void displayLoopEnd() {
  display.display();
#ifdef DEBUG
  Serial.println();
#endif
}

void lowPowerDisplay(byte onOrOff) {
  if (onOrOff) {
    display.ssd1306_command(SSD1306_DISPLAYON); // To switch display on
  } else {
    display.ssd1306_command(SSD1306_DISPLAYOFF); // To switch display off
  }
}

byte wheelDraw(byte last) {
  // Display slider at bottom of screen, comparing it with the last value to reduce unexpectedly high settings
  display.setCursor(10, 10);
  byte val = readScrollWheel() / 4;
  handlePrints("PWM_D: ", val);

  if ((val - 50) <= last) {  // Hold previous value until slider is close or below the previous value
    last = val;
  }
  display.drawLine(0,                                     SCREEN_HEIGHT - 1,
                   (last / (256 / SCREEN_WIDTH)) + 8,    SCREEN_HEIGHT - 1, SSD1306_WHITE);
  display.drawLine((val / (256 / SCREEN_WIDTH)) - 8,    SCREEN_HEIGHT - 2,
                   (val / (256 / SCREEN_WIDTH)) + 8,    SCREEN_HEIGHT - 2, SSD1306_WHITE);
  return last;
}

void buttonDraw(byte val[], byte n) {
  // Display button presses at bottom of screen
  static byte anim[4];
  byte i;
  for (i = 0; i < n; ++i) {
    anim[i] = anim[i] * val[i] + 2; // Sets anim[i] to 0 if button not pressed, increment if pressed
    if (anim[i] > 8) {
      anim[i] = 8;
    }
    if (val[i] != 0) {
      display.drawLine(SCREEN_WIDTH / 4 * i - anim[i] + 12,       SCREEN_HEIGHT - 1,
                       SCREEN_WIDTH / 4 * (i + 1) + anim[i] - 12, SCREEN_HEIGHT - 1, SSD1306_WHITE);
    }
  }
}

void initialiseDisplay() {
  // Turn display on
  pinMode(OLED_ENABLE, OUTPUT);
  pinMode(MODE, INPUT_PULLUP);
  digitalWrite(OLED_ENABLE, HIGH);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Display start for brownout detection
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println(F("Initialising..!"));
  display.display();

  // Set brightness
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(0);
}

void loadingBars(int i) {
  display.drawLine(i, 0, i + 1, 0, SSD1306_WHITE);
  display.drawLine(SCREEN_WIDTH - i, SCREEN_HEIGHT - 1, SCREEN_WIDTH - i - 1, SCREEN_HEIGHT - 1, SSD1306_WHITE);
  if (i > (SCREEN_WIDTH / 2)) {
    display.drawLine(i, 0, i + 2, 5, SSD1306_WHITE);
    display.drawLine(i + 1, 0, i + 3, 5, SSD1306_WHITE);
    display.drawLine(SCREEN_WIDTH - i, SCREEN_HEIGHT - 1, SCREEN_WIDTH - i - 2, SCREEN_HEIGHT - 6, SSD1306_WHITE);
    display.drawLine(SCREEN_WIDTH - i - 1, SCREEN_HEIGHT - 1, SCREEN_WIDTH - i - 3, SCREEN_HEIGHT - 6, SSD1306_WHITE);
  }
}
