/*
 *  libsensor - Arduino library to interface with generic serial sensor.
 */
#include "libsensor.h"


Sensor::Sensor()
{
    /* Clear buffer. */
    memset(&buf, 0, IO_BUFSIZE);
}


Sensor::Sensor(HardwareSerial *s, uint32_t b)
{
    /* Get serial communication parameters. */
    serial = s;
    baud = b;
    /* Clear buffer. */
    memset(&buf, 0, IO_BUFSIZE);
}


void
Sensor::begin(void)
{
    /* Initialize serial device. */
    serial->begin(baud);
    serial_is_active = true;
}


/* Convert 4 bytes to float. */
float
Sensor::b2f(uint8_t idx)
{
    float rv;
    if (idx + 3 >= IO_BUFSIZE) {
        /* Return 0 for invalid results. */
        rv = 0;
    }
    uint8_t b[] = {buf[idx + 3], buf[idx + 2], buf[idx + 1], buf[idx]};
    memcpy(&rv, &b, sizeof(float));
    return rv;
}
