/*!
 *  \file serialsensor.hh
 *  \brief Serial IO Sensor interface.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016-2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license BSD 2-Clause License
 *
 * This file is provided for instructional value only.  It is not guaranteed for any particular purpose.  
 * The authors do not offer any warranties or representations, nor do they accept any liabilities with respect 
 * to the information or their use.  This file is distributed with the understanding that the  authors are not engaged 
 * in rendering engineering or other professional services associate with their use.
 */
#pragma once


// 3rd party
#include <QByteArray>
#include <QDebug>
#include <QIODevice>
#include <QObject>
#include <QPointer>
#include <QSerialPort>
#include <QSerialPortInfo>
// dfti
#include "core/qptrutil.hh"
#include "settings/settings.hh"
#include "util/util.hh"


namespace dfti {


//! Base class for interfacing with sensors over a serial port (UART/RS-232).
class SerialSensor : public QObject
{
    Q_OBJECT;

public:
    //! Constructor
    /*!
     *  \param _settings Pointer to settings object.
     *  \param _parent Pointer to parent QObject.
     */
    explicit SerialSensor(Settings *_settings, QObject* _parent = nullptr) :
        settings(_settings), QObject(_parent) { };

    //! Destructor
    ~SerialSensor();

    //! Set the serial port parameters.
    /*!
     *  \param _portName The serial port name.
     */
    void configureSerial(QString _portName);

    //! Initialize the serial port.
    void init();

    //! Returns true if the serial port is open.
    /*!
     *  \return True if the serial port is open.
     */
    bool isOpen(void);

    //! Opens the serial port.
    /*!
     *  Since we may not initially know which serial port corresponds to which
     *  sensor, we need to wait to open the serial port associated with the
     *  sensor until we know which port it is. Once we do, we call this open
     *  method to start reading the serial port.
     */
    virtual void open(void);

    //! Set the serial port baud rate.
    /*!
     *  \param rate The serial port baud rate. Must be one of 57600, 115200.
     *  \remark If an unsupported baud rate is given, the sensor falls back to
     *      57600 baud.
     */
    void setBaudRate(quint32 rate);

    //! Start the sensor in a thread.
    void threadStart(void);

public slots:
    //! Slot to read in data over serial and parse complete packets.
    virtual void readData(void) = 0;

protected:
    //! Settings object.
    QPointer<Settings> settings = nullptr;

    //! Serial port name.
    QString portName{""};

    //! Serial port baud rate.
    QSerialPort::BaudRate baudRate{QSerialPort::Baud115200};

    //! Indicates if serial port passed validation.
    bool _valid_serial = false;

    //! Serial port object.
    QPointer<QSerialPort> _port = nullptr;

    //! Validates a proposed serial port.
    /*!
     *  Checks to see if the given serial port name is a valid serial port.
     *
     *  \param _port Proposed serial port.
     *  \return True if the port name corresponds to a valid system serial port.
     */
    QString validateSerialPort(QString _port);
};


};  // namespace dfti
