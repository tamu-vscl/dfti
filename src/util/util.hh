/*!
 *  \file util.hh
 *  \brief Utility functions.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016-2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#pragma once


// stdlib
#include <ctime>
#include <sys/time.h>
// 3rd party
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>


namespace dfti {


//! Get timestamp in microseconds.
/*!
 *  \remark If you are using this function on an embedded computer without a
 *      Real Time Clock, this is probably the time since boot.
 *  \return Unix time, microseconds since Jan 1, 1970.
 */
quint64 getTimeUsec(void);


//! Convert GPS timestamp in nanoseconds to Unix timestamp in microseconds.
/*!
 *  \param gpsTime Timestamp from GPS epoch (0000 6 JAN 1980) in nanoseconds.
 */
quint64 gpsToUnixUsec(quint64 gpsTime);


//! Convert GPS timestamp in nanoseconds to Unix timestamp in seconds.
/*!
 *  \param gpsTime Timestamp from GPS epoch (0000 6 JAN 1980) in nanoseconds.
 *  \remark This function is used to generate a timestamp to update the system
 *      time of an embedded computer without an RTC using date(1).
 */
quint64 gpsToUnixSec(quint64 gpsTime);


//! Convert Hertz rate to microsecond sampling time.
/*!
 *  \param rate Sampling rate in Hz.
 *  \return Sampling period in microseconds.
 */
float hzToUsec(quint8 rate);


//! Validates a proposed serial port.
/*!
 *  Checks to see if the given serial port name is a valid serial port.
 *
 *  \param _port Proposed serial port.
 *  \return True if the port name corresponds to a valid system serial port.
 */
QString validateSerialPort(QString _port);


};  // namespace dfti
