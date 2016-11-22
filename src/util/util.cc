/*!
 *  \file util.cc
 *  \brief Utility function implementations.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#include "util.hh"


namespace dfti {


quint64
getTimeUsec(void)
{
    static struct timeval _ts;
    gettimeofday(&_ts, nullptr);
    return 1e6 * _ts.tv_sec + _ts.tv_usec;
}


float
hzToUsec(quint8 rate)
{
    return 1e6 / static_cast<float>(rate);
}


QString
validateSerialPort(QString _port)
{
    for (auto port : QSerialPortInfo::availablePorts()) {
        QString candidate = port.portName();
        candidate.prepend("/dev/");
        if (_port == candidate) {
            return _port;
        }
    }
    qWarning() << "[WARN]  validation of serial port" << _port << "failed!";
    return QString{""};
}


};  // namespace dfti
