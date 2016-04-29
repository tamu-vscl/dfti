/*
 *  libadc - Arduino library to parse Aeroprobe Micro Air Data Computer.
 */
#ifndef __LIBADC_H
#define __LIBADC_H


#include "Arduino.h"
#include "HardwareSerial.h"
#include "String.h"
#include "libsensor.hh"


#define ADC_PKT_SZ     57
#define ADC_MSG_LEN    52
#define ADC_CKSUM_BYTE 54
#define ADC_VEL_START   7
#define ADC_AOA_START  14
#define ADC_BET_START  22
#define ADC_PREC        2


class ADC
{
public:
    /* Read sensor data. */
    void read();

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
    bool parse(void);

    /* Air data. */
    float _airspeed = 0;
    float _alpha = 0;
    float _beta = 0;
};

#endif
