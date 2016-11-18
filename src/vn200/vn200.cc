/*!
 *  \file vn200.cc
 *  \brief VectorNav VN-200 Inertial Navigation System implementation.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#include "vn200.hh"


namespace dfti {


// ----------------------------------------------------------------------------
//  Constructors/destructors
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  Public functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Public Slots
// ----------------------------------------------------------------------------
void
VN200::readData(void)
{
    // Flags for parsing.
    bool cleanup = false;
    bool completePacket = false;

    // Read in a single byte.
    quint8 ch;
    if (!_port->getChar((char *) &ch)) {
        if (_debug) {
            qDebug() << "Failed to read serial port!";
        }
        cleanup = true;
    }

    // Attempt to construct valid packet.
    if (!foundSyncByte && (ch == syncByte)) {
        buf[currentBufIdx++] = ch;
        foundSyncByte = true;
    } else if (foundSyncByte) {
        /*
         * If we haven't found the group byte yet, then check to see if the
         * current byte is the group byte. Since we know a priori what the
         * packet from the IMU will look like we can check for the known group
         * byte directly.
         */
        if (!foundGroupByte && (ch == groupByte)) {
            buf[currentBufIdx++] = ch;
            foundGroupByte = true;
        /*
         * If we've found the group byte, the next byte should be part of the
         * packet and we add it to the buffer.
         */
        } else if (foundGroupByte) {
            if (currentBufIdx < packetSize) {
                buf[currentBufIdx++] = ch;
                if (currentBufIdx == packetSize) {
                    // We are at the end of the packet, so we should validate
                    // and parse it.
                    completePacket = true;
                }
            } else {
                // Packet is longer than buffer size so probably invalid,
                // ignore.
                cleanup = true;
            }
        } else {
            // Garbage data, reset buffer and flags.
            cleanup = true;
        }

    } else {
        // Garbage data, reset buffer and flags.
        cleanup = true;
    }

    // Validate and parse the packet if we have an entire packet.
    if (completePacket) {
        if (validateVN200Checksum(buf, packetSize)) {
            packet = reinterpret_cast<VN200Packet*>(buf);
            copyPacketToData();
            // Emit the signal.
            emit measurementUpdate(data);
            // If we are in the verbose debugging mode, print the parsed data.
            if (_debug) {
                qDebug() << "TimeGPS :" << data.gpsTimeNs
                         << "Quaternion: {"
                         << data.quaternion[0] << ","
                         << data.quaternion[1] << ","
                         << data.quaternion[2] << ","
                         << data.quaternion[3] << "}"
                         << "P:" << data.angularRatesRPS[0]
                         << "Q:" << data.angularRatesRPS[1]
                         << "R:" << data.angularRatesRPS[2]
                         << "Lat:" << data.posDegDegM[0]
                         << "Lon:" << data.posDegDegM[1]
                         << "Alt:" << data.posDegDegM[2]
                         << "Vx:" << data.velNedMps[0]
                         << "Vy:" << data.velNedMps[1]
                         << "Vz:" << data.velNedMps[2]
                         << "Ax:" << data.accelMps2[0]
                         << "Ay:" << data.accelMps2[1]
                         << "Az:" << data.accelMps2[2]
                         << "Mx:" << data.mag[0]
                         << "My:" << data.mag[1]
                         << "Mz:" << data.mag[2]
                         << "Temp:" << data.tempC
                         << "Pressure:" << data.pressureKpa;
            }
        } else {
            if (_debug) {
                qDebug() << "[INFO]    packet failed validation";
            }
        }
        cleanup = true;
    }

    // Reset the buffer if we're done with the packet or had an error.
    if (cleanup) {
        // Clear buffer for new data.
        std::memset(buf, 0, packetSize);
        // Reset buffer index.
        currentBufIdx = 0;
        // Reset flags.
        foundSyncByte = false;
        foundGroupByte = false;
    }

    return;
}

// ----------------------------------------------------------------------------
//  Private functions
// ----------------------------------------------------------------------------
void
VN200::copyPacketToData(void)
{
    // Stop if the packet is nullptr.
    if (packet == nullptr) {
        return;
    }
    // Copy data.
    // GPS Time
    data.gpsTimeNs = packet->timeGPS;
    // Quaternion: note that DFTI using scalar first, VN uses scalar last so we
    // swap...
    data.quaternion[0] = packet->quaternion[3];  // scalar
    data.quaternion[1] = packet->quaternion[0];  // vector 0
    data.quaternion[2] = packet->quaternion[1];  // vector 1
    data.quaternion[3] = packet->quaternion[2];  // vector 2
    // Angular Rates, Pos, Vel, Accel, Mag.
    for (quint8 i = 0; i < 3; ++i) {
        data.angularRatesRPS[i] = packet->angularRate[i];
        data.posDegDegM[i] = packet->position[i];
        data.velNedMps[i] = packet->velocity[i];
        data.accelMps2[i] = packet->accel[i];
        data.mag[i] = packet->mag[i];
    }
    // Temp
    data.tempC = packet->temp;
    data.pressureKpa = packet->pressure;
    return;
}


// ----------------------------------------------------------------------------
//  Functions
// ----------------------------------------------------------------------------
bool
validateVN200Checksum(quint8 *pkt, quint8 pktLen)
{
    quint16 crc = 0;
    // Calculate checksum.
    for (quint8 i = 1; i < pktLen; ++i) {
        crc = (quint8) (crc >> 8) | (crc << 8);
        crc ^= pkt[i];
        crc ^= (quint8) (crc & 0xff) >> 4;
        crc ^= crc << 12;
        crc ^= (crc & 0x00ff) << 5;
    }
    // If the CRC is 0, then the validation passed.
    return crc ? false : true;
}


};  // namespace dfti
