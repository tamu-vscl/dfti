/*!
 *  \file uadc.cc
 *  \brief Aeroprobe Micro Air Data Computer implementation.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */


// 3rd party
#include <QDebug>
#include <QIODevice>
#include <QSerialPortInfo>
// dfti
#include "uadc.hh"
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
        if (_debug) {
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
            QByteArray _id_buf = pkt.left(5);
            data.id = _id_buf.toInt();
            // Indicated Airspeed
            QByteArray _ias_mps_buf = pkt.mid(uadcPktIasPos, uadcPktIasLen);
            data.ias_mps = _ias_mps_buf.toFloat();
            // Angle-of-Attack
            QByteArray _aoa_deg_buf = pkt.mid(uadcPktAoAPos, uadcPktAoALen);
            data.aoa_deg = _aoa_deg_buf.toFloat();
            // Sideslip Angle
            QByteArray _aos_deg_buf = pkt.mid(uadcPktAoSPos, uadcPktAoSLen);
            data.aos_deg = _aos_deg_buf.toFloat();
            // Pressure Altitude
            QByteArray _alt_m_buf = pkt.mid(uadcPktAltPos, uadcPktAltLen);
            data.alt_m = _alt_m_buf.toInt();
            // Total Pressure
            QByteArray _pt_pa_buf = pkt.mid(uadcPktPtPos, uadcPktPtLen);
            data.pt_pa = _pt_pa_buf.toInt();
            // Static Pressure
            QByteArray _ps_pa_buf = pkt.mid(uadcPktPsPos, uadcPktPsLen);
            data.ps_pa = _ps_pa_buf.toInt();
            // Emit the signal.
            emit measurementUpdate(data);
            // If we are in the verbose debugging mode, print the parsed data.
            if (_debug) {
                qDebug() << "ID :" << data.id
                         << "IAS:" << data.ias_mps
                         << "AoA:" << data.aoa_deg
                         << "AoS:" << data.aos_deg
                         << "ALT:" << data.alt_m
                         << "Pt :" << data.pt_pa
                         << "Ps :" << data.ps_pa;
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
//  Functions
// ----------------------------------------------------------------------------
bool
validateUADCChecksum(QByteArray pkt)
{
    bool ok;
    quint8 cksum = 0;
    // Extract checksum byte.
    QByteArray _cksum_bytes = pkt.mid(uadcPktCksumPos, 2);
    quint8 cksum_byte = static_cast<quint8>(_cksum_bytes.toInt(&ok, 16));
    // Calculate checksum.
    for (quint8 i = 0; i < uadcPktCksumPos; ++i) {
        cksum ^= static_cast<quint8>(pkt.at(i));
    }
    // if (_debug) {
        qDebug() << "checksum byte:" << cksum_byte
                 << "calc. checksum:" << cksum;
    // }
    return ok ? (cksum == cksum_byte ? true : false) : false;
}


};  // namespace dfti
