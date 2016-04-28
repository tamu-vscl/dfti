/*
 *  libvn200 - Arduino library to parse VN-200 INS data.
 */
#include "libvn200.hh"


VN200::VN200(HardwareSerial &s, uint32_t b)
{
    /* Get serial communication parameters. */
    serial = s;
    baud = b;
    /* Clear buffer. */
    memset(&buf, 0, VN200_BUFSIZE);
}


void
VN200::begin(void)
{
    /* Initialize serial device. */
    serial->begin(baud);
    serial_is_active = true;
}


void
VN200::read(void)
{
    if (!serial_is_active) {
        /* Serial port is not active, so do nothing. */
        return;
    }

    uint8_t len = serial->available();
    if (len > 0) {
        for (uint8_t i = 0; i < len; ++i) {
            uint8_t b = serial->read();
            if ((b == VN200_SYNC) && crc()) {
                parse();
                bufidx = 0;
                buf[bufidx++] = b;
            } else {
                if (bufidx < VN200_INS_PKT_SZ) {
                    buf[bufidx++] = b;
                } else {
                    /* Buffer overflow. */
                    bufidx = 0;
                }
            }
        }
    }
}


/* Parse data from packets. */
bool
VN200::parse(void)
{
    if (buf[1] == VN200_INS_PKT) {
        /* Extract Euler angles and body-axis attitude rates. */
        _yaw = b2f(4);
        _pitch = b2f(8);
        _roll = b2f(12);
        _p = b2f(28);
        _q = b2f(32);
        _r = b2f(36);
    }
}


/* Calculate CRC for the message, and check it against the CRC value. */
bool
VN200::crc(void)
{
    if (bufidx < VN200_INS_PKT_SZ) {
        return false;
    }
    uint16_t crc = 0x0000;
    for (uint8_t i = 1; i < bufidx - 1; ++i) {
        crc = (uint16_t)((uint8_t) (crc >> 8) | (crc << 8));
        crc ^= (uint16_t) buf[i];
        crc ^= (uint8_t) (crc & 0xff) >> 4;
        crc ^= crc << 12;
        crc ^= (crc & 0x00ff) << 5;
    }
    return crc == 0x0000 ? true : false;
}


/* Convert 4 bytes to float. */
float
VN200::b2f(uint8_t idx)
{
    float rv;
    uint8_t b[4];
    if (idx + 3 >= VN200_BUFSIZE) {
        /* Return 0 for invalid results. */
        rv = 0;
    }
    b = {buf[idx + 3], buf[idx + 2], buf[idx + 1], buf[idx]};
    memcpy(&rv, &b, sizeof(float));
    return rv;
}


/* Return yaw angle in degrees. */
float
yaw(void)
{
    return _yaw;
}


/* Return pitch angle in degrees. */
float
pitch(void)
{
    return _pitch;
}


/* Return roll angle in degrees. */
float
roll(void)
{
    return _roll;
}


/* Return body-axis roll rate in degrees/sec. */
float
p(void)
{
    return _p;
}


/* Return body-axis pitch rate in degrees/sec. */
float
q(void)
{
    return _q;
}


/* Return body-axis yaw rate in degrees/sec. */
float
r(void)
{
    return _r;
}
