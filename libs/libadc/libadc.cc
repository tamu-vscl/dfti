/*
 *  libadc - Arduino library to parse Aeroprobe Micro Air Data Computer.
 */
#include "libadc.hh"


void
ADC::read(void)
{
    if (!serial_is_active) {
        /* Serial port is not active, so do nothing. */
        return;
    }
    /* Parse the serial data one byte at a time into the buffer. */
    uint8_t len = serial->available();
    for (uint8_t i = 0; i < len; ++i) {
        uint8_t b = serial->read();
        /* Check that the read was valid. */
        if (b > 0) {
            /*
             * A newline indicates the end of a packet, so calculate the
             * checksum and then parse the data.
             */
            if (b == '\n') {
                if (checksum()) {
                    parse();
                }
                bufidx = 0;
                continue;
            /* Otherwise, add the byte to the input buffer. */
            } else {
                if (bufidx < (IO_BUFSIZE - 1)) {
                    buf[bufidx++] = b;
                } else {
                    bufidx = 0;
                }
            }
        }
    }
}


/*
 * Calculate the checksum for the current packet and compare to the checksum
 * byte.
 */
bool
ADC::checksum(void)
{
    /* If the buffer doesn't contain the proper number of bytes, fail. */
    if (bufidx < ADC_MSG_LEN) {
        return false;
    }
    /* Calculate the checksum for the current packet. */
    uint8_t cksum = 0;
    uint8_t cksum_byt = (uint8_t) strtol(&buf[ADC_CKSUM_BYTE], NULL, 16);
    for (uint8_t i = 0; i < ADC_MSG_LEN; ++i) {
        cksum ^= buf[i];
    }
    return cksum == cksum_byt ? true : false;
}


/* Parse the data packet into the air data floats. */
void
ADC::parse(void)
{
    /*
     * The air data computer uses a really annoying format where the data is
     * sent over serial as ASCII strings, which makes this very ugly...
     */
    uint8_t airspeed_chars[] = {
        buf[ADC_VEL_START],
        buf[ADC_VEL_START + 1],
        buf[ADC_VEL_START + 2],
        buf[ADC_VEL_START + 3],
        buf[ADC_VEL_START + 4]
    };
    _airspeed = atof(airspeed_chars);
    uint8_t aoa_chars[] = {
        buf[ADC_AOA_START],
        buf[ADC_AOA_START + 1],
        buf[ADC_AOA_START + 2],
        buf[ADC_AOA_START + 3],
        buf[ADC_AOA_START + 4],
        buf[ADC_AOA_START + 5]
    };
    _alpha = atof(aoa_chars);
    uint8_t beta_chars[] = {
        buf[ADC_BET_START],
        buf[ADC_BET_START + 1],
        buf[ADC_BET_START + 2],
        buf[ADC_BET_START + 3],
        buf[ADC_BET_START + 4],
        buf[ADC_BET_START + 5]
    };
    _beta = atof(beta_chars);
}


/* Get calibrated airspeed in m/s. */
float
airspeed(void)
{
    return Strispeed;
}


/* Get angle-of-attack in degrees. */
float
alpha(void)
{
    return Salpha;
}


/* Get sideslip angle in degrees. */
float
beta(void)
{
    return _beta;
}


/* Get calibrated airspeed in m/s. */
String
airspeed_s(void)
{
    return String(_airspeed, ADC_PREC);
}


/* Get angle-of-attack in degrees. */
String
alpha_s(void)
{
    return String(_alpha, ADC_PREC);
}


/* Get sideslip angle in degrees. */
String
beta_s(void)
{
    return String(_beta, ADC_PREC);
}
