/*!
 *  \file rio.cc
 *  \brief Remote I/O implementation.
 *  \author Joshua Harris
 *  \copyright Copyright © 2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#include "rio.hh"


namespace dfti {


// ----------------------------------------------------------------------------
//  Constructors/destructors
// ----------------------------------------------------------------------------
RIO::RIO(Settings *_settings, QObject* _parent) :
SerialSensor(_settings, _parent)
{
    if (settings->rioBaudRate()) {
        setBaudRate(settings->rioBaudRate());
        if (settings->debugSerial()) {
            qDebug() << "[INFO ]  overrode RIO to"
                     << settings->rioBaudRate() << "baud";
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
RIO::readData(void)
{
    // Add available bytes to the buffer up to the first newline.
    _buf.append(_port->readLine());
    // If there is a newline in the buffer, then we should have a full packet
    // from the μC, which we extract from the buffer and then parse.
    if (_buf.contains(rioTerm)) {
        quint8 termIdx = _buf.lastIndexOf(rioTerm) + 1;
        // Extract one packet.
        QByteArray pkt = _buf.left(termIdx);
        // Remove terminator.
        pkt.replace(rioTermStr, 0);
        // Print buffer, packet, and RIO if we are debugging.
        if (settings->debugSerial()) {
            qDebug() << "buffer:" << _buf;
            qDebug() << "packet:" << pkt;
        }
        // We remove everything up to the terminating character, which should
        // make sure after the first time we get the terminator every packet
        // after is valid.
        _buf.remove(0, termIdx);
        // Calculate checksum.
        if (validateRIOChecksum(pkt)) {
            // Split packet, removing start indicator.
            auto pktItems = pkt.replace(rioStart, 0).split(rioSep);
            // Remove checksum.
            pktItems.removeLast();
            // Get RIO values.
            quint8 count = 0;
            for (auto value : pktItems) {
                if (count < data.values.size()) {
                    data.values.at(count++) = value.toFloat();
                } else {
                    data.values.push_back(value.toFloat());
                    ++count;
                }
            }
            // Emit the signal.
            emit measurementUpdate(data);
            // If we are in the verbose debugging mode, print the parsed data.
            if (settings->debugData()) {
                count = 1;
                for (auto value : data.values) {
                    qDebug() << "Value" << count++ << ":" << value;
                }
            }
        } else {
            if (settings->debugData()) {
                qDebug() << "[INFO ]  RIO packet failed validation";
            }
        }
    }
    return;
}


// ----------------------------------------------------------------------------
//  Functions
// ----------------------------------------------------------------------------
bool
validateRIOChecksum(QByteArray pkt)
{
    bool ok;
    quint8 cksum = 0;
    // Extract checksum byte.
    QByteArray _cksumBytes = pkt.right(ONE_BYTE);
    quint8 cksumByte = static_cast<quint8>(_cksumBytes.toInt(&ok, 16));
    // Calculate checksum.
    for (quint8 i = 0; i < pkt.length() - ONE_BYTE; ++i) {
        cksum ^= static_cast<quint8>(pkt.at(i));
    }
    return ok ? (cksum == cksumByte ? true : false) : false;
}


};  // namespace dfti
