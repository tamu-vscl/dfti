/*!
 *  \file uadc.cc
 *  \brief Aeroprobe Micro Air Data Computer implementation.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016-2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license BSD 2-Clause License
 */
#include "uadc.hh"


namespace dfti {


// ----------------------------------------------------------------------------
//  Constructors/destructors
// ----------------------------------------------------------------------------
uADC::uADC(Settings *_settings, QObject* _parent) :
SerialSensor(_settings, _parent)
{
    if (settings->uADCBaudRate()) {
        setBaudRate(settings->uADCBaudRate());
        if (settings->debugSerial()) {
            qDebug() << "[INFO ]  overrode uADC to"
                     << settings->uADCBaudRate() << "baud";
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
uADC::readData(void)
{
    // qDebug() << "cur bytes" << _port->bytesAvailable();
    // Add available bytes to the buffer up to the first newline.
    _buf.append(_port->readLine());
    // If there is a newline in the buffer, then we should have a full packet
    // from the uADC, which we extract from the buffer and then parse.
    if (_buf.contains(uadcTerm)) {
        // We assume that the packet in the buffer is exactly the packet
        // length. This may not be true when we start out, in which case the
        // packet will fail validation.
        QByteArray pkt = _buf.left(uadcPktLen);
        if (settings->debugSerial()) {
            qDebug() << "buffer:" << _buf;
            qDebug() << "packet:" << pkt;
        }
        // We remove everything up to the terminating character, which should
        // make sure after the first time we get the terminator every packet
        // after is valid.
        _buf.remove(0, _buf.lastIndexOf(uadcTerm) + 1);
        // Validate the packet and parse the data structure. If validation
        // fails, then display a warning.
        if (validateUADCChecksum(pkt)) {
            // Parse the data structure.
            // Packet ID
            QByteArray _idBuf = pkt.left(5);
            data.id = _idBuf.toInt();
            // Indicated Airspeed
            QByteArray _iasMpsBuf = pkt.mid(uadcPktIasPos, uadcPktIasLen);
            data.iasMps = _iasMpsBuf.toFloat();
            // Angle-of-Attack
            QByteArray _aoaDegBuf = pkt.mid(uadcPktAoAPos, uadcPktAoALen);
            data.aoaDeg = _aoaDegBuf.toFloat();
            // Sideslip Angle
            QByteArray _aosDegBuf = pkt.mid(uadcPktAoSPos, uadcPktAoSLen);
            data.aosDeg = _aosDegBuf.toFloat();
            // Pressure Altitude
            QByteArray _altMBuf = pkt.mid(uadcPktAltPos, uadcPktAltLen);
            data.altM = _altMBuf.toInt();
            // Total Pressure
            QByteArray _ptPaBuf = pkt.mid(uadcPktPtPos, uadcPktPtLen);
            data.ptPa = _ptPaBuf.toInt();
            // Static Pressure
            QByteArray _psPaBuf = pkt.mid(uadcPktPsPos, uadcPktPsLen);
            data.psPa = _psPaBuf.toInt();
            // Emit the signal.
            emit measurementUpdate(data);
            // If we are in the verbose debugging mode, print the parsed data.
            if (settings->debugData()) {
                qDebug() << "ID :" << data.id
                         << "IAS:" << data.iasMps
                         << "AoA:" << data.aoaDeg
                         << "AoS:" << data.aosDeg
                         << "ALT:" << data.altM
                         << "Pt :" << data.ptPa
                         << "Ps :" << data.psPa;
            }
        } else {
            if (settings->debugData()) {
                qDebug() << "[INFO ]  packet failed validation";
            }
        }
    }
    return;
}


// ----------------------------------------------------------------------------
//  Functions
// ----------------------------------------------------------------------------
bool
validateUADCChecksum(QByteArray pkt)
{
    bool ok;
    quint8 cksum = 0;
    // Extract checksum byte.
    QByteArray _cksumBytes = pkt.mid(uadcPktCksumPos, 2);
    quint8 cksumByte = static_cast<quint8>(_cksumBytes.toInt(&ok, 16));
    // Calculate checksum.
    for (quint8 i = 0; i < uadcPktCksumPos; ++i) {
        cksum ^= static_cast<quint8>(pkt.at(i));
    }
    return ok ? (cksum == cksumByte ? true : false) : false;
}


};  // namespace dfti
