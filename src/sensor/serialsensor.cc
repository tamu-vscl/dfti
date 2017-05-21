/*!
 *  \file serialsensor.cc
 *  \brief Serial I/O sensor interface implementation.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016-2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license BSD 2-Clause License
 */
#include "serialsensor.hh"


namespace dfti {


// ----------------------------------------------------------------------------
//  Constructors/destructors
// ----------------------------------------------------------------------------
SerialSensor::~SerialSensor(void)
{
    if (_port->isOpen()) {
        _port->close();
    }
}


// ----------------------------------------------------------------------------
//  Public functions
// ----------------------------------------------------------------------------
void
SerialSensor::configureSerial(QString _portName)
{
    portName = _portName;
}


void
SerialSensor::init()
{
    QString port = validateSerialPort(portName);
    _port = new QSerialPort(this);
    if (port != "") {
        _port->setPortName(port);
        if (_port->setBaudRate(baudRate) &&
            _port->setDataBits(QSerialPort::Data8) &&
            _port->setParity(QSerialPort::NoParity) &&
            _port->setStopBits(QSerialPort::OneStop) &&
            _port->setFlowControl(QSerialPort::NoFlowControl)) {
            if (settings->debugSerial()) {
                qDebug() << "[INFO ]  port settings successful";
            }
            _valid_serial = true;
        } else {
            _valid_serial = false;
            qWarning() << "[ERROR]  port settings failed";
        }
    }
    return;
}


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
        connect(QSERIALPORTPTR(_port), &QIODevice::readyRead, this,
            &SerialSensor::readData);
    }
}


bool
SerialSensor::isOpen(void)
{
    return _port != nullptr ? _port->isOpen() : false;
}


void
SerialSensor::setBaudRate(quint32 rate)
{
    switch (rate) {
        case 115200:
            baudRate = QSerialPort::Baud115200;
            break;
        case 57600:  // fallthrough
        default:
            baudRate = QSerialPort::Baud57600;
            break;
    }
}


void
SerialSensor::threadStart(void)
{
    init();
    open();
}


QString
SerialSensor::validateSerialPort(QString _port)
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
