/*
 *  libsensor - Arduino library to interface with generic serial sensor.
 *  Note that this essentially provides an abstract base class.
 */
#ifndef __LIBSENSOR_H
#define __LIBSENSOR_H


#include "Arduino.h"
#include "HardwareSerial.h"


#define IO_BUFSIZE 128


class Sensor
{
public:
    Sensor(HardwareSerial &s, uint32_t baud);
    ~Sensor();
    /* Start the serial port. */
    void begin(void);
    /* Read sensor data. */
    virtual void read(void);

protected:
    /* Check to see if CRC/checksum is valid. */
    virtual bool checksum(void);
    /* Convert bytes to float. Used to be in libbc. */
    float b2f(uint8_t idx);

    /* Serial device the VN-200 is connected to. */
    HardwareSerial *serial = NULL;
    bool serial_is_active = false;
    /* Buffer. */
    uint8_t bufidx = 0;  /* Index for current byte in buffer. */
    uint8_t buf[IO_BUFSIZE];
};


#endif
