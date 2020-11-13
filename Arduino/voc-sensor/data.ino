#define DATA_LEN 64  // 64 should be default

unsigned int co2[DATA_LEN];
unsigned int tvoc[DATA_LEN];
int data = 0;   // Counter for circular buffer array


unsigned int avg[DATA_LEN];
int avg_counter = 0;

void incrementDataCounter() {
  ++data;
  if (data >= DATA_LEN) {
    data = 0;
    avg[avg_counter] = calculateAverage();
    ++avg_counter;
    if (avg_counter >= DATA_LEN) {
      avg_counter = 0;
    }
  }
}

int calculateAverage() {
  unsigned int average = 0;
  int i;
  for (i = 0; i < DATA_LEN; ++i) {
    average += co2[i];
  }
  average = average / DATA_LEN;
  return (int) average;

}
