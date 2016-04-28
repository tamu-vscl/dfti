/*
 *  libvn200 - Arduino library to parse VN-200 INS data.
 */
#ifndef __LIBVN200_H
#define __LIBVN200_H


#include "Arduino.h"
#include "HardwareSerial.h"
#include "libsensor.hh"


#define VN200_INS_PKT       1
#define VN200_INS_PKT_SZ   42
#define VN200_SYNC       0xFA


class VN200
{
public:
    /* Read sensor data. */
    void read(void);

    /* Get sensor outputs. */
    float yaw(void) const;
    float pitch(void) const;
    float roll(void) const;
    float p(void) const;
    float q(void) const;
    float r(void) const;

private:
    /* Calculate the CRC/checksum. */
    bool checksum(void);
    /* Parse data packets. */
    void parse(void);

    /* IMU data. */
    float _yaw = 0;
    float _pitch = 0;
    float _roll = 0;
    float _p = 0;
    float _q = 0;
    float _r = 0;
};


#endif
