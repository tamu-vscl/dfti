/*!
 *  \file vn200.cc
 *  \brief VectorNav VN-200 Inertial Navigation System implementation.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */


// stdlib
#include <cstring>
// 3rd party
#include <QDebug>
#include <QIODevice>
#include <QSerialPortInfo>
// dfti
#include "vn200.hh"
#include "util/util.hh"


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
    // Add available bytes to the buffer up to the first newline.
    _buf.append(_port->read(2 * packetSize));
    quint8 bufSize = _buf.size();
    if (_buf.contains(header) && bufSize >= packetSize) {
        // Find the start of the first full packet we have.
        quint8 startPos = _buf.indexOf(header);
        if (startPos + packetSize < bufSize) {
            // We don't have a full packet, so delay.
            return;
        }
        QByteArray pkt = _buf.mid(startPos, packetSize);
        if (_debug) {
            qDebug() << "buffer:" << _buf;
            qDebug() << "packet:" << pkt;
        }
        // Remove everything up to the end of this packet.
        _buf.remove(0, startPos + packetSize);
        // Validate and parse the packet. Emit signal for measurement update.
        if (validateVN200Checksum(pkt)) {
            packet = reinterpret_cast<VN200Packet*>(pkt.data());
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
validateVN200Checksum(QByteArray pkt)
{
    quint16 crc = 0;
    // Calculate checksum.
    for (quint8 i = 0; i < pkt.size(); ++i) {
        crc = (quint8) (crc >> 8) | (crc << 8);
        crc ^= pkt.at(i);
        crc ^= (quint8) (crc & 0xff) >> 4;
        crc ^= crc << 12;
        crc ^= (crc & 0x00ff) << 5;
    }
    // If the CRC is 0, then the validation passed.
    return crc ? false : true;
}


};  // namespace dfti
