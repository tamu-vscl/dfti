/*!
 *  \file vn200.cc
 *  \brief VectorNav VN-200 Inertial Navigation System implementation.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016-2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#include "vn200.hh"


namespace dfti {


// ----------------------------------------------------------------------------
//  Constructors/destructors
// ----------------------------------------------------------------------------
VN200::VN200(Settings *_settings, QObject* _parent) :
SerialSensor(_settings, _parent)
{
    if (settings->vn200BaudRate()) {
        setBaudRate(settings->vn200BaudRate());
        if (settings->debugSerial()) {
            qDebug() << "[INFO ]  overrode VN-200 to"
                     << settings->vn200BaudRate() << "baud";
        }
    }
}

// ----------------------------------------------------------------------------
//  Public functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Public Slots
// ----------------------------------------------------------------------------
void
VN200::readData(void)
{
    // Add available bytes to the buffer.
    buf.append(_port->readAll());

    if (buf.contains(header)) {
        quint16 startIdx = buf.indexOf(header);
        // Make sure we have a full packet, otherwise return and wait.
        if (buf.size() < startIdx + packetSize) {
            return;
        }
        QByteArray pkt = buf.mid(startIdx, packetSize);

        //Validate packet.
        if (validateVN200Checksum(pkt)) {
            packet = reinterpret_cast<VN200Packet*>(pkt.data());
            copyPacketToData();
            // Emit the measurement update signal.
            emit measurementUpdate(data);
            // Check to see if we have GPS. If either the latitude or longitude
            // is nonzero we should be OK.
            if (abs(data.posDegDegM[0]) || abs(data.posDegDegM[1])) {
                emit gpsAvailable(true);
            }
            // If we are in the verbose debugging mode, print the parsed data.
            if (settings->debugData()) {
                qDebug() << "TimeGPS :" << data.gpsTimeNs
                         << "Yaw" << data.eulerDeg[0]
                         << "Pitch" << data.eulerDeg[1]
                         << "Roll" << data.eulerDeg[2]
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
                         << "Az:" << data.accelMps2[2];
            }
        } else {
            if (settings->debugData()) {
                qDebug() << "[INFO ]  packet failed validation";
            }
        }
        // We remove everything up to the next packet.
        buf.remove(0, startIdx + packetSize);
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
        data.eulerDeg[i] = packet->euler[i];
        data.angularRatesRPS[i] = packet->angularRate[i];
        data.posDegDegM[i] = packet->position[i];
        data.velNedMps[i] = packet->velocity[i];
        data.accelMps2[i] = packet->accel[i];
    }
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
    for (quint8 i = 1; i < pkt.size(); ++i) {
        crc = static_cast<quint8>(crc >> 8) | (crc << 8);
        crc ^= static_cast<quint8>(pkt[i]);
        crc ^= static_cast<quint8>(crc & 0xff) >> 4;
        crc ^= crc << 12;
        crc ^= (crc & 0x00ff) << 5;
    }
    // If the CRC is 0, then the validation passed.
    return crc ? false : true;
}


};  // namespace dfti
