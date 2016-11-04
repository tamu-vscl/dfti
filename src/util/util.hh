/*!
 *  \file util.hh
 *  \brief Utility functions.
 *  \author Joshua Harris
 *  \copyright ISC License
 */
#pragma once


// 3rd party
#include <QDebug>
#include <QSerialPort>
#include <QString>


namespace dfti {


//! Validates a proposed serial port.
/*!
 *  Checks to see if the given serial port name is a valid serial port.
 *
 *  \param _port Proposed serial port.
 *  \return True if the port name corresponds to a valid system serial port.
 */
QString validateSerialPort(QString _port);


};  // namespace dfti