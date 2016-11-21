/*!
 *  \file serialsensor.cc
 *  \brief Serial I/O sensor interface implementation.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#include "serialsensor.hh"


namespace dfti {


// ----------------------------------------------------------------------------
//  Constructors/destructors
// ----------------------------------------------------------------------------
SerialSensor::SerialSensor(Settings *_settings, QObject* _parent)
: settings(_settings), QObject(_parent)
{
    // Initialize the serial port object, but do not assign the port yet.
    _port = new QSerialPort(this);
    if (_port->setBaudRate(QSerialPort::Baud115200) &&
        _port->setDataBits(QSerialPort::Data8) &&
        _port->setParity(QSerialPort::NoParity) &&
        _port->setStopBits(QSerialPort::OneStop) &&
        _port->setFlowControl(QSerialPort::NoFlowControl)) {
        if (settings->debugSerial()) {
            qDebug() << "[INFO]    port settings successful";
        }
    } else {
        if (settings->debugSerial()) {
            qDebug() << "[ERROR]   port settings failed";
        }
    }
    return;
}


SerialSensor::~SerialSensor(void)
{
    if (_port->isOpen()) {
        _port->close();
    }
    delete _port;
    settings = nullptr;
}


// ----------------------------------------------------------------------------
//  Public functions
// ----------------------------------------------------------------------------
void
SerialSensor::open(void)
{
    if (_valid_serial && !isOpen()) {
        if (_port->open(QIODevice::ReadOnly)) {
            if (settings->debugSerial()) {
                qDebug() << "Opened serial port:"
                         << _port->portName();
            }
        } else {
            if (settings->debugSerial()) {
                qDebug() << "Failed to open serial port:"
                         << _port->errorString();
            }
        };
        connect(_port, &QIODevice::readyRead, this,
            &SerialSensor::readData);
    }
}

void
SerialSensor::open(QSerialPort::BaudRate baud)
{
    if (_port->setBaudRate(baud)) {
        if (_valid_serial && !isOpen()) {
            if (_port->open(QIODevice::ReadOnly)) {
                if (settings->debugSerial()) {
                    qDebug() << "Opened serial port:"
                             << _port->portName();
                }
            } else {
                if (settings->debugSerial()) {
                    qDebug() << "Failed to open serial port:"
                             << _port->errorString();
                }
            };
            connect(_port, &QIODevice::readyRead, this,
                &SerialSensor::readData);
        }
    } else {
        qWarning() << "Failed to update baud rate.";
    }
}


bool
SerialSensor::isOpen(void)
{
    return _port != nullptr ? _port->isOpen() : false;
}


void
SerialSensor::setSerialPort(QString port)
{
    // Check if the serial port is valid.
    port = validateSerialPort(port);
    if (port != "") {
        _valid_serial = true;
        if (!isOpen()) {
            _port->setPortName(port);
            if (settings->debugSerial()) {
                qDebug() << "using serial port" << port;
            }
        }
    } else {
        if (_port->portName() == "") {
            _valid_serial = false;
        }
    }
    return;
}


};  // namespace dfti
