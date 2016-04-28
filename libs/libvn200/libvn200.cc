/*
 *  libvn200 - Arduino library to parse VN-200 INS data.
 */
#include "libvn200.hh"


VN200::VN200(HardwareSerial &s, uint32_t b)
{
    // Get serial communication parameters.
    serial = s;
    baud = b;
    // Clear buffer.
    memset(&buf, 0, VN200_BUFSIZE);
}


void
VN200::begin(void)
{
    // Initialize serial device.
    serial->begin(baud);
    serial_is_active = true;
}


void
VN200::read(void)
{
    if (!serial_is_active) {
        // Serial port is not active, so do nothing.
        return;
    }

    uint8_t len = serial->available();
    if (len) {
        // Prevent buffer overflow.
        len = len > VN200_BUFSIZE ? VN200_BUFSIZE : len;
        serial->readBytes(buf, len);
        if (buf[0] == VN200_INS_PACKET) {
            // TODO: Parse this.
        }
    }
}


uint16_t
VN200::crc(uint8_t _buf[], uint8_t _len)
{
    // Taken from VectorNav sample code.
    uint16_t crc = 0x0000;
    for (uint8_t i = 0; i < _len; ++i) {
        crc = (uint16_t)((uint8_t) (crc >> 8) | (crc << 8));
        crc ^= (uint16_t) _buf[i];
        crc ^= (uint8_t) (crc & 0xff) >> 4;
        crc ^= crc << 12;
        crc ^= (crc & 0x00ff) << 5;
    }
    return crc;
}
