/*
 *  libvn200 - Arduino library to parse VN-200 INS data.
 */
#ifndef __LIBVN200_H
#define __LIBVN200_H


#include "Arduino.h"
#include "HardwareSerial.h"


#define VN200_BUFSIZE    128
#define VN200_INS_PKT      1
#define VN200_INS_PKT_SZ  42


class VN200
{
public:
    VN200(HardwareSerial &s, uint32_t baud);
    ~VN200();
    // Start the serial port.
    void begin();
    // Read sensor data.
    void read();

private:
    // Calculate the CRC/checksum.
    uint16_t crc(uint8_t _buf[], uint8_t _len);

    // Serial device the VN-200 is connected to.
    HardwareSerial *serial = NULL;
    bool serial_is_active = false;
    // Buffer.
    uint8_t buf[VN200_BUFSIZE];
    // IMU data.
    float yaw = 0;
    float pitch = 0;
    float roll = 0;
    float p = 0;
    float q = 0;
    float r = 0;
};

#endif
