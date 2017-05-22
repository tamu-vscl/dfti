/*!
 *  \file server.cc
 *  \brief Data server interface implementation.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016-2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license BSD 2-Clause License
 */
#include "server.hh"


namespace dfti {


// ----------------------------------------------------------------------------
//  Constructors/destructors
// ----------------------------------------------------------------------------
Server::Server(Settings *_settings, QObject* _parent)
: settings(_settings), QObject(_parent)
{
    // Create UDP socket.
    socket = new QUdpSocket(this);

    // Get address and port from settings.
    address = settings->serverAddress();
    port = settings->serverPort();
}


Server::~Server()
{
}

// ----------------------------------------------------------------------------
//  Public functions
// ----------------------------------------------------------------------------
void
Server::start(void)
{
    writeTimer = new QTimer(this);
    connect(QTIMERPTR(writeTimer), &QTimer::timeout, this, &Server::writeData);
    writeTimer->start(settings->sendRateMs());
}


void
Server::enableRIO(RIO *rio)
{
    connect(rio, &RIO::measurementUpdate, this, &Server::getRIOData);
}


void
Server::enableUADC(uADC *adc)
{
    connect(adc, &uADC::measurementUpdate, this, &Server::getUADCData);
}


void
Server::enableVN200(VN200 *ins)
{
    connect(ins, &VN200::measurementUpdate, this, &Server::getVN200Data);
}

// ----------------------------------------------------------------------------
// Public Slots
// ----------------------------------------------------------------------------
void
Server::getRIOData(RIOData data)
{
    quint8 size = data.values.size();
    stateData.numRIOValues = size > STATE_DATA_SIZE ? STATE_DATA_SIZE : size;
    for (quint8 i = 0; i < stateData.numRIOValues; ++i) {
      stateData.rioValues[i] = data.values[i];
    }
    if (settings->debugSerial()) {
        qDebug() << "Server::getRIOData";
    }
}


void
Server::getUADCData(uADCData data)
{
    stateData.iasMps = data.iasMps;
    stateData.aoaDeg = data.aoaDeg;
    stateData.aosDeg = data.aosDeg;
    if (settings->debugSerial()) {
        qDebug() << "Server::getuADCData";
    }
}


void
Server::getVN200Data(VN200Data data)
{
    stateData.gpsTimeNs = data.gpsTimeNs;
    for (quint8 i = 0; i < 3; ++i) {
        stateData.eulerDeg[i] = data.eulerDeg[i];
        stateData.quaternion[i] = data.quaternion[i];
        stateData.angularRatesRPS[i] = data.angularRatesRPS[i];
        stateData.accelMps2[i] = data.accelMps2[i];
    }
    stateData.quaternion[3] = data.quaternion[3];
    if (settings->debugSerial()) {
        qDebug() << "Server::getVN200Data";
    }
}


void
Server::writeData(void)
{
    // Create packet.
    // TODO: do this in a safer C++ style.
    const int len = sizeof(StateData);
    char data[len];
    memset(&data, 0, len);
    memcpy(&data, &stateData, len);

    qint64 bytes = socket->writeDatagram(data, len, address, port);
    if (bytes < 0) {
        qDebug() << "Server:writeData: Error writing data:" << socket->error();
    }

    if (settings->debugSerial()) {
        qDebug() << "Server:writeData";
    }
}

// ----------------------------------------------------------------------------
//  Private functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  Functions
// ----------------------------------------------------------------------------


};  // namespace dfti
