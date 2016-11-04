/*!
 *  \file util.hh
 *  \brief Utility function implementations.
 *  \author Joshua Harris <joshua.a.harris@tamu.edu>
 *  \copyright ISC License
 */


// 3rd party
#include<QSerialPortInfo>
// project
#include "util.hh"


namespace dfti {

QString
validateSerialPort(QString _port)
{
    for (auto port : QSerialPortInfo::availablePorts()) {
        QString candidate = port.portName();
        if (_port.contains(candidate)) {
            return _port;
        }
    }
    qWarning() << "[WARN]  validation of serial port" << _port << "failed!";
    return QString{""};
}


};  // namespace dfti
