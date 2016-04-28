/*
 *  libadc - Arduino library to parse Aeroprobe Micro Air Data Computer.
 */
#ifndef __LIBADC_H
#define __LIBADC_H


#include "Arduino.h"
#include "HardwareSerial.h"


#define ADC_BUFSIZE   128
#define ADC_PKT_SZ     57
#define ADC_MSG_LEN    52
#define ADC_CKSUM_BYTE 54
#define ADC_VEL_START   7
#define ADC_AOA_START  14
#define ADC_BET_START  22


class ADC
{
public:
    ADC(HardwareSerial &s, uint32_t baud);
    ~ADC();
    /* Start the serial port. */
    void begin();
    /* Read sensor data. */
    void read();
    /* Output air data. */
    float airspeed(void) const;
    float alpha(void) const;
    float beta(void) const;

private:
    /* Calculate the checksum. */
    bool checksum(void);
    /* Parse the data. */
    bool parse(void);

    /* Serial device the uADC is connected to. */
    HardwareSerial *serial = NULL;
    bool serial_is_active = false;
    /* Buffer. */
    uint8_t bufidx = 0;
    uint8_t buf[ADC_BUFSIZE];
    /* Air data. */
    float _airspeed = 0;
    float _alpha = 0;
    float _beta = 0;
};

#endif
