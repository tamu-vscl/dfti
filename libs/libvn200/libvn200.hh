/*
 *  libvn200 - Arduino library to parse VN-200 INS data.
 */
#ifndef __LIBVN200_H
#define __LIBVN200_H


#include "Arduino.h"
#include "HardwareSerial.h"


#define VN200_BUFSIZE     128
#define VN200_INS_PKT       1
#define VN200_INS_PKT_SZ   42
#define VN200_SYNC       0xFA


class VN200
{
public:
    VN200(HardwareSerial &s, uint32_t baud);
    ~VN200();
    /* Start the serial port. */
    void begin();
    /* Read sensor data. */
    void read();
    /* Get sensor outputs. */
    float yaw(void) const;
    float pitch(void) const;
    float roll(void) const;
    float p(void) const;
    float q(void) const;
    float r(void) const;

private:
    /* Calculate the CRC/checksum. */
    bool crc(void);
    /* Parse data packets. */
    void parse(void);
    /* Convert bytes to float. Used to be in libbc. */
    float b2f(uint8_t idx);

    /* Serial device the VN-200 is connected to. */
    HardwareSerial *serial = NULL;
    bool serial_is_active = false;
    /* Buffer. */
    uint8_t bufidx = 0;
    uint8_t buf[VN200_BUFSIZE];
    /* IMU data. */
    float _yaw = 0;
    float _pitch = 0;
    float _roll = 0;
    float _p = 0;
    float _q = 0;
    float _r = 0;
};

#endif
