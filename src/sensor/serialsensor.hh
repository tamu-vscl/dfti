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
     *  \param _baud The serial port baud rate (default 115k).
     */
    void configureSerial(QString _portName,
        QSerialPort::BaudRate _baud = QSerialPort::Baud115200);

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

    //! Start the sensor in a thread.
    void threadStart(void);


public slots:
    //! Slot to read in data over serial and parse complete packets.
    virtual void readData(void) = 0;


protected:
    //! Settings object.
    Settings *settings = nullptr;

    //! Serial port name.
    QString portName{""};

    //! Serial port baud rate.
    QSerialPort::BaudRate baudRate{QSerialPort::Baud115200};

    //! Indicates if serial port passed validation.
    bool _valid_serial = false;

    //! Serial port object.
    QSerialPort* _port = nullptr;
};


};  // namespace dfti
