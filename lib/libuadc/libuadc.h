/*
 *  libadc - Arduino library to parse Aeroprobe Micro Air Data Computer.
 */
#ifndef __LIBADC_H
#define __LIBADC_H


#include <Arduino.h>
#include <libsensor.h>


#define UADC_PKT_SZ     57
#define UADC_MSG_LEN    52
#define UADC_CKSUM_BYTE 54
#define UADC_VEL_START   7
#define UADC_AOA_START  14
#define UADC_BET_START  22
#define UADC_PREC        2


class UADC : public Sensor
{
public:
    explicit UADC() : Sensor() { };
    explicit UADC(HardwareSerial *s, uint32_t b) : Sensor(s, b) { };
    /* Read sensor data. */
    int8_t read();

    /* Output air data. */
    float airspeed(void) const;
    float alpha(void) const;
    float beta(void) const;
    String airspeed_s(void) const;
    String alpha_s(void) const;
    String beta_s(void) const;

private:
    /* Calculate the checksum. */
    bool checksum(void);
    /* Parse the data. */
    void parse(void);

    /* Air data. */
    float _airspeed = 0;
    float _alpha = 0;
    float _beta = 0;
};

#endif
