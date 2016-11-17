/*!
 *  \file uadc.cc
 *  \brief Aeroprobe Micro Air Data Computer implementation.
 *  \author Joshua Harris
 *  \copyright ISC License
 */


// 3rd party
#include <QDebug>
#include <QIODevice>
#include <QSerialPortInfo>
// project
#include "uadc.hh"
#include "util/util.hh"


namespace dfti {


// ----------------------------------------------------------------------------
//  Constructors/destructors
// ----------------------------------------------------------------------------
uAirDataComputer::uAirDataComputer(bool d, QObject* _parent)
: _debug(d), QObject(_parent)
{
    // Initialize the serial port object, but do not assign the port yet.
    _port = new QSerialPort(this);
    // The uADC is 115200 baud, 8-N-1.
    if (_port->setBaudRate(QSerialPort::Baud115200) &&
        _port->setDataBits(QSerialPort::Data8) &&
        _port->setParity(QSerialPort::NoParity) &&
        _port->setStopBits(QSerialPort::OneStop) &&
        _port->setFlowControl(QSerialPort::NoFlowControl)) {
        if (_debug) {
            qDebug() << "[INFO]    port settings successful";
        }
    } else {
        if (_debug) {
            qDebug() << "[ERROR]   port settings failed";
        }
    }
    return;
}


uAirDataComputer::~uAirDataComputer(void)
{
    if (_port->isOpen()) {
        _port->close();
    }
    delete _port;
}


// ----------------------------------------------------------------------------
//  Public functions
// ----------------------------------------------------------------------------
void
uAirDataComputer::open(void)
{
    if (_valid_serial && !isOpen()) {
        if (_port->open(QIODevice::ReadOnly)) {
        } else {
            if (_debug) {
                qDebug() << "Failed to open serial port:"
                         << _port->errorString();
            }
        };
        connect(_port, &QIODevice::readyRead, this,
            &uAirDataComputer::readData);
    }
}


bool
uAirDataComputer::isOpen(void)
{
    return _port != nullptr ? _port->isOpen() : false;
}


// void
// print_state(void)
// {
//     qDebug() << "IAS:" << _ias_mps << ""
// }


void
uAirDataComputer::setSerialPort(QString port)
{
    // Check if the serial port is valid.
    port = validateSerialPort(port);
    if (_debug) {
        qDebug() << "using serial port" << port;
    }
    if (port != "") {
        _valid_serial = true;
        _port->setPortName(port);
    }
    return;
}


// ----------------------------------------------------------------------------
// Public Slots
// ----------------------------------------------------------------------------
void
uAirDataComputer::readData(void)
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
            quint32 id = _id_buf.toInt();
            // Indicated Airspeed
            QByteArray _ias_mps_buf = pkt.mid(uadcPktIasPos, uadcPktIasLen);
            _ias_mps = _ias_mps_buf.toFloat();
            // Angle-of-Attack
            QByteArray _aoa_deg_buf = pkt.mid(uadcPktAoAPos, uadcPktAoALen);
            _aoa_deg = _aoa_deg_buf.toFloat();
            // Sideslip Angle
            QByteArray _aos_deg_buf = pkt.mid(uadcPktAoSPos, uadcPktAoSLen);
            _aos_deg = _aos_deg_buf.toFloat();
            // Pressure Altitude
            QByteArray _alt_m_buf = pkt.mid(uadcPktAltPos, uadcPktAltLen);
            _alt_m = _alt_m_buf.toInt();
            // Total Pressure
            QByteArray _pt_pa_buf = pkt.mid(uadcPktPtPos, uadcPktPtLen);
            _pt_pa = _pt_pa_buf.toInt();
            // Static Pressure
            QByteArray _ps_pa_buf = pkt.mid(uadcPktPsPos, uadcPktPsLen);
            _ps_pa = _ps_pa_buf.toInt();
            // If we are in the verbose debugging mode, print the parsed data.
            if (_debug) {
                qDebug() << "ID :" << id
                         << "IAS:" << _ias_mps
                         << "AoA:" << _aoa_deg
                         << "AoS:" << _aos_deg
                         << "ALT:" << _alt_m
                         << "Pt :" << _pt_pa
                         << "Ps :" << _ps_pa;
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
    if (_debug) {
        qDebug() << "checksum byte:" << cksum_byte
                 << "calc. checksum:" << cksum;
    }
    return ok ? (cksum == cksum_byte ? true : false) : false;
}


};  // namespace dfti
