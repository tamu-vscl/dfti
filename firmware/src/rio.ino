/*
 *  \file rio.ino
 *  \brief Read encoders from Super Cub using Arduino and send over serial.
 *  \author Vinicius Goecks and Joshua Harris
 *  \copyright Copyright © 2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
*/

// initialize readings
int n_enc = 5; // number of encoder
int enc[5] = {0,0,0,0,0};
int i = 0;

// temporary array to store formatted checksum byte
char tmp[3];

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
  String msg_str = "$$$";  // message start token
  for(i = 0; i < n_enc; i++) {
    msg_str.concat(enc[i]);
    msg_str.concat("$");
  }

  // get message string as byte array to calculate checksum
  const char *msg = msg_str.c_str();

  // calculate checksum as bitwise XOR of each byte
  unsigned short cksum = 0;
  for (i = 0; i < msg_str.length(); ++i) {
    cksum ^= msg[i];
  }

  // format checksum byte as padded hex and add to string
  sprintf(tmp, "%02x", cksum);
  msg_str.concat(tmp);

  // send packet over serial
  Serial.println(msg_str);
}
