/*!
 *  \file serialsensor.hh
 *  \brief Serial IO Sensor interface.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#pragma once


// 3rd party
#include <QByteArray>
#include <QDebug>
#include <QIODevice>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
// dfti
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
     *  \param d Boolean to turn on qDebug output.
     *  \param _parent Pointer to parent QObject.
     *  \todo Replace d boolean with debug mode enum.
     */
    explicit SerialSensor(Settings *_settings, QObject* _parent = nullptr);

    //! Destructor
    ~SerialSensor();

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
    void open(void);

    //! Opens the serial port while changing the baud rate.
    /*!
     *  Since we may not initially know which serial port corresponds to which
     *  sensor, we need to wait to open the serial port associated with the
     *  sensor until we know which port it is. Once we do, we call this open
     *  method to start reading the serial port. This version of open supports
     *  changing the baud rate if the serial port is not 115200 baud as assumed
     *  by default.
     *  \param baud The baud rate as a QSerialPort::BaudRate enum.
     */
    void open(QSerialPort::BaudRate baud);

    //! Set the serial port name.
    /*!
     *  Sets the serial port name once we have figured out which serial port
     *  corresponds to the sensor.
     *
     *  \param port The serial port name.
     */
    void setSerialPort(QString port);


public slots:
    //! Slot to read in data over serial and parse complete packets.
    virtual void readData(void) = 0;


protected:
    //! Settings object.
    Settings *settings = nullptr;

    //! Do we display verbose debug output?
    bool _debug = false;

    //! Indicates if serial port passed validation.
    bool _valid_serial = false;

    //! Serial port object.
    QSerialPort* _port = nullptr;
};


};  // namespace dfti
