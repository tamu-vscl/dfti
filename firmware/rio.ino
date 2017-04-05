/*
  Read 7 encoders from Super Cub using Arduino Mega and send to Beaglebone over serial.

  Vinicius Goecks
  February 13, 2017
*/

// initialize readings
int n_enc = 5; // number of encoder
int enc[5] = {0,0,0,0,0};
int i = 0;

void setup() {
  // initialize serial communication
  Serial.begin(112500);
}

void loop() {
  // read the input on analog pin
  for(i = 0; i < n_enc; i++) {
    // read encoder
    enc[i] = analogRead(i);
    // add delay to settle adc
    delay(2);
  }

  // map to degrees
  for(i = 0; i < n_enc; i++) {
    // apparently there's a dead band  before 39 and after 982 on the pots
    enc[i] = map(enc[i], 39, 982, 0, 360);
  }

  // send over serial (test on terminal)
  Serial.print("$$$"); // start message
  for(i = 0; i < n_enc; i++) {
    Serial.print(enc[i]);
    Serial.print("$");
  }
  Serial.println();
  //Serial.println(enc[6]);
}
