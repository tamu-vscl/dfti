/*
 *  libUADC - Arduino library to parse Aeroprobe Micro Air Data Computer.
 */
#include "libuadc.h"


int8_t
UADC::read(void)
{
    int8_t rv = 0;
    if (!serial_is_active) {
        /* Serial port is not active, so do nothing. */
        return -EINACTIVE;
    }

    uint8_t len = serial->available();
    if (!len) {
        return -ENODATA;
    }

    serial->readBytesUntil('\n', buf, UADC_PKT_SZ);
    if (checksum()) {
        parse();
    } else {
        rv = -EINVAL;
    }

    return rv;
}


/*
 * Calculate the checksum for the current packet and compare to the checksum
 * byte.
 */
bool
UADC::checksum(void)
{
    /* Calculate the checksum for the current packet. */
    uint8_t cksum = 0;
    uint8_t cksum_byt = (uint8_t) strtol((const char *) &buf[UADC_CKSUM_BYTE],
                                         NULL, 16);
    for (uint8_t i = 0; i < UADC_CKSUM_BYTE; ++i) {
        cksum ^= buf[i];
    }
    return cksum == cksum_byt ? true : false;
}


/* Parse the data packet into the air data floats. */
void
UADC::parse(void)
{
    /*
     * The air data computer uses a really annoying format where the data is
     * sent over serial as ASCII strings, which makes this very ugly...
     */
    uint8_t airspeed_chars[] = {
        buf[UADC_VEL_START],
        buf[UADC_VEL_START + 1],
        buf[UADC_VEL_START + 2],
        buf[UADC_VEL_START + 3],
        buf[UADC_VEL_START + 4]
    };
    _airspeed = atof((const char *) airspeed_chars);
    uint8_t aoa_chars[] = {
        buf[UADC_AOA_START],
        buf[UADC_AOA_START + 1],
        buf[UADC_AOA_START + 2],
        buf[UADC_AOA_START + 3],
        buf[UADC_AOA_START + 4],
        buf[UADC_AOA_START + 5]
    };
    _alpha = atof((const char *) aoa_chars);
    uint8_t beta_chars[] = {
        buf[UADC_BET_START],
        buf[UADC_BET_START + 1],
        buf[UADC_BET_START + 2],
        buf[UADC_BET_START + 3],
        buf[UADC_BET_START + 4],
        buf[UADC_BET_START + 5]
    };
    _beta = atof((const char *) beta_chars);
}


/* Get calibrated airspeed in m/s. */
float
UADC::airspeed(void) const
{
    return _airspeed;
}


/* Get angle-of-attack in degrees. */
float
UADC::alpha(void) const
{
    return _alpha;
}


/* Get sideslip angle in degrees. */
float
UADC::beta(void) const
{
    return _beta;
}


/* Get calibrated airspeed in m/s. */
String
UADC::airspeed_s(void) const
{
    return String(_airspeed, UADC_PREC);
}


/* Get angle-of-attack in degrees. */
String
UADC::alpha_s(void) const
{
    return String(_alpha, UADC_PREC);
}


/* Get sideslip angle in degrees. */
String
UADC::beta_s(void) const
{
    return String(_beta, UADC_PREC);
}
