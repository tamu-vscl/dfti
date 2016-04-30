/*
 *  libvn200 - Arduino library to parse VN-200 INS data.
 */
#ifndef __LIBVN200_H
#define __LIBVN200_H


#include <Arduino.h>
#include <errno.h>
#include <libsensor.h>


#define VN200_INS_PKT         1
#define VN200_INS_PKT_SZ     42
#define VN200_PREC            4
#define VN200_SYNC       '\xFA'


class VN200 : public Sensor
{
public:
    explicit VN200() : Sensor() { };
    explicit VN200(HardwareSerial *s, uint32_t b) : Sensor(s, b) { };
    /* Read sensor data. */
    int8_t read(void);

    /* Get sensor outputs. */
    float yaw(void) const;
    float pitch(void) const;
    float roll(void) const;
    float p(void) const;
    float q(void) const;
    float r(void) const;
    String yaw_s(void) const;
    String pitch_s(void) const;
    String roll_s(void) const;
    String p_s(void) const;
    String q_s(void) const;
    String r_s(void) const;

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
