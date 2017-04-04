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


quint64
gpsToUnixUsec(quint64 gpsTime)
{
    const float nsToUs = 1e-3;
    const quint64 gpsEpochFromUnixEpochUs = 315964800000000;
    return static_cast<quint64>(gpsEpochFromUnixEpochUs + gpsTime * nsToUs);
}


quint64
gpsToUnixSec(quint64 gpsTime)
{
    const float usToSec = 1e-6;
    return static_cast<quint64>(gpsToUnixUsec(gpsTime) * usToSec);
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
    qWarning() << "[WARN ]  validation of serial port" << _port << "failed!";
    return QString{""};
}


};  // namespace dfti
