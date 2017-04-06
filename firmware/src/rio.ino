/*
 *  \file rio.ino
 *  \brief Read encoders from Super Cub using Arduino and send over serial.
 *  \author Vinicius Goecks and Joshua Harris
 *  \copyright Copyright © 2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
*/
#include <SoftwareSerial.h>
#include <FreqCount.h>

// initialize readings
int n_enc = 5; // number of encoder
int enc[5] = {0,0,0,0,0};
int i = 0;

// temporary array to store formatted checksum byte
char tmp[3];

// rpm constants and initialization
float rpm_ratio = 1.0; // this is the ratio between the sensor and the
                       // final stage. example: gear boxes, etc
float mistery_factor = 308880/43600; // correction factor calculated comparing
                                     // tachometer reading and sensor value.
static unsigned long rpm_freq = 0;
uint32_t             rpm_send = 0;


void setup() {
  // initialize serial communication and FreqCount for rpm
  Serial.begin(112500);
  FreqCount.begin(1000);
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

  // read rpm
  rpm_freq = FreqCount.read();

  // the brushless sensor triggers 1~10 pulses per second when no RPM is detected. erase them.
  rpm_send = (rpm_freq > 10) ? rpm_freq * 60.0 / rpm_ratio : 0;

  // apply mistery factor to match tacho reading and append to message
  rpm_send = rpm_send / mistery_factor;
  msg_str.concat(rpm_send);
  msg_str.concat("$");

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
