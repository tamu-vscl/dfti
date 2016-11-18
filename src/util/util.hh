/*!
 *  \file util.hh
 *  \brief Utility functions.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
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
quint64 getTimeUsec(void);


//! Validates a proposed serial port.
/*!
 *  Checks to see if the given serial port name is a valid serial port.
 *
 *  \param _port Proposed serial port.
 *  \return True if the port name corresponds to a valid system serial port.
 */
QString validateSerialPort(QString _port);


};  // namespace dfti
