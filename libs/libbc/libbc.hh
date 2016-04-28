/*
 *  libbc - Header only byte conversions.
 */
#include <stdint.h>


// Convert byte array to float.
float
b2f(uint8_t &buf, uint8_t idx, uint8_t bufsz)
{
    float rv;
    uint8_t b[4];
    if (idx + 3 >= bufsz) {
        // Return 0 for invalid results.
        rv = 0;
    }
    b = {buf[idx + 3], buf[idx + 2], buf[idx + 1], buf[idx]};
    memcpy(&rv, &b, sizeof(float));
    return rv;
}
