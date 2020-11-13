

byte outputs[4] = {0, 0, 0, 0};

void setOutputs(byte channel, byte value) {
  outputs[channel] = value;
  
  analogWrite( OUT_A, outputs[0]);
  analogWrite( OUT_B, outputs[1]);
  analogWrite( OUT_C, outputs[2]);
  analogWrite( OUT_D, outputs[3]);
}

byte readOutputs(byte channel) {
  return outputs[channel];
}

void initialiseOutputs() {
  pinMode( OUT_A, OUTPUT);
  pinMode( OUT_B, OUTPUT);
  pinMode( OUT_C, OUTPUT);
  pinMode( OUT_D, OUTPUT);
}
