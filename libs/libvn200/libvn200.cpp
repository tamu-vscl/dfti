/*
 *  libvn200 - Arduino library to parse VN-200 INS data.
 */
#include "libvn200.h"


int8_t
VN200::read(void)
{
    if (!serial_is_active) {
        /* Serial port is not active, so do nothing. */
        return -EINACTIVE;
    }

    uint8_t len = serial->available();
    if (!len) {
        return -ENODATA;
    }
    if (len > 0) {
        for (uint8_t i = 0; i < len; ++i) {
            uint8_t b = serial->read();
            if ((b == VN200_SYNC) && checksum()) {
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
    return 0;
}


/* Parse data from packets. */
void
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
VN200::checksum(void)
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


/* Return yaw angle in degrees. */
float
VN200::yaw(void) const
{
    return _yaw;
}


/* Return pitch angle in degrees. */
float
VN200::pitch(void) const
{
    return _pitch;
}


/* Return roll angle in degrees. */
float
VN200::roll(void) const
{
    return _roll;
}


/* Return body-axis roll rate in degrees/sec. */
float
VN200::p(void) const
{
    return _p;
}


/* Return body-axis pitch rate in degrees/sec. */
float
VN200::q(void) const
{
    return _q;
}


/* Return body-axis yaw rate in degrees/sec. */
float
VN200::r(void) const
{
    return _r;
}


/* Return yaw angle in degrees. */
String
VN200::yaw_s(void) const
{
    return String(_yaw, VN200_PREC);
}


/* Return pitch angle in degrees. */
String
VN200::pitch_s(void) const
{
    return String(_pitch, VN200_PREC);
}


/* Return roll angle in degrees. */
String
VN200::roll_s(void) const
{
    return String(_roll, VN200_PREC);
}


/* Return body-axis roll rate in degrees/sec. */
String
VN200::p_s(void) const
{
    return String(_p, VN200_PREC);
}


/* Return body-axis pitch rate in degrees/sec. */
String
VN200::q_s(void) const
{
    return String(_q, VN200_PREC);
}


/* Return body-axis yaw rate in degrees/sec. */
String
VN200::r_s(void) const
{
    return String(_r, VN200_PREC);
}
