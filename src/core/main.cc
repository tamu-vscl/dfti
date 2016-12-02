/*!
 *  \file main.cc
 *  \brief DFTI main program.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */


// 3rd party
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QMetaType>
#include <QObject>
#include <QString>
#include <QThread>
// dfti
#include "autopilot/autopilot.hh"
#include "consts.hh"
#include "logger.hh"
#include "uadc/uadc.hh"
#include "util/util.hh"
#include "vn200/vn200.hh"


//! Identify serial ports corresponding to sensors.
/*!
 *  \param settings Reference to global settings object.
 *  \param ap Pointer to Autopilot class instance.
 *  \param uadc Pointer to uADC class instance.
 *  \param vn200 Pointer to VN200 class instance.
 *  \remark The pointers should be set to nullptr if the corresponding sensor
 *      is not in use.
 */
void
identifySerialPorts(const dfti::Settings& settings, dfti::Autopilot *ap,
    dfti::uADC *uadc, dfti::VN200 *vn200)
{
    const quint8 oneByte{1};
    const quint8 serialBufSize{255};

    // Check to see if we have overriden serial ports.
    bool detectAP{true};
    bool detectUADC{true};
    bool detectVN200{true};
    if (settings.useMavlink() && (settings.autopilotSerialPort() != "")) {
        detectAP = false;
        ap->configureSerial(settings.autopilotSerialPort());
        qDebug() << "MAVLink/Pixhawk serial port set to"
                 << settings.autopilotSerialPort();
    }
    if (settings.useUADC() && (settings.uADCSerialPort() != "")) {
        detectUADC = false;
        uadc->configureSerial(settings.uADCSerialPort());
        qDebug() << "uADC serial port set to" << settings.uADCSerialPort();
    }
    if (settings.useVN200() && (settings.vn200SerialPort() != "")) {
        detectVN200 = false;
        vn200->configureSerial(settings.vn200SerialPort());
        qDebug() << "VN-200 serial port set to" << settings.vn200SerialPort();
    }
    // Stop if we have all sensors configured already.
    if (!(settings.useMavlink() && detectAP) &&
        !(settings.useUADC() && detectUADC) &&
        !(settings.useVN200() && detectVN200)) {
        return;
    }

    // Serial port vendor/product IDs.
    quint16 thisPortPID{0};
    quint16 thisPortVID{0};
    quint16 pixhawkPID{17};
    quint16 pixhawkVID{9900};

    // Packet contents to help determine payload type.
    QByteArray vn200Header{"\xfa\x01\xf2\x05"};
    QByteArray mavlinkSync{"\xfe"};

    // Create a serial port to connect to candidate ports.
    QSerialPort sp;
    sp.setBaudRate(QSerialPort::Baud115200);
    sp.setDataBits(QSerialPort::Data8);
    sp.setParity(QSerialPort::NoParity);
    sp.setStopBits(QSerialPort::OneStop);
    sp.setFlowControl(QSerialPort::NoFlowControl);

    // Iterate through the available serial ports.
    for (auto port : QSerialPortInfo::availablePorts()) {
        // Skip the default system serial ports.
        if (port.portName().contains("ttyS")) {
            continue;
        }
        // If we are debugging, print the port information for device serial
        // port candidates.
        if (settings.debugSerial()) {
            qDebug() << "[PORT " << port.systemLocation() << "]";
            qDebug() << "\tdescription:           " << port.description();
            qDebug() << "\thasProductIdentifier:  "
                     << port.hasProductIdentifier();
            qDebug() << "\tproductIdentifier:     "
                     << port.productIdentifier();
            qDebug() << "\thasVendorIdentifier:   "
                     << port.hasVendorIdentifier();
            qDebug() << "\tvendorIdentifier:      " << port.vendorIdentifier();
            qDebug() << "\tmanufacturer:          " << port.manufacturer();
            qDebug() << "\tportName:              " << port.portName();
            qDebug() << "\tserialNumber:          " << port.serialNumber();
        }
        // Get vendor and product IDs.
        if (port.hasVendorIdentifier()) {
            thisPortVID = port.vendorIdentifier();
        }
        if (port.hasProductIdentifier()) {
            thisPortPID = port.productIdentifier();
        }
        // Check if we have a Pixhawk USB(?) serial connection.
        if (settings.useMavlink() && detectAP) {
            if ((thisPortPID == pixhawkPID) && (thisPortVID == pixhawkVID)) {
                if (ap != nullptr) {
                    ap->configureSerial(port.systemLocation());
                    qDebug() << "MAVLink/Pixhawk serial port set to"
                             << port.systemLocation() << "(Pixhawk USB)";
                }
                continue;
            }
        }
        // Attempt to read in data from the serial port to determine what it
        // is.
        sp.setPortName(port.systemLocation());
        if (sp.isOpen()) {
            sp.close();
        }
        if (sp.open(QIODevice::ReadOnly)) {
            QByteArray data;
            // Read in a buffer of data.
            bool dataAvailable = true;
            while (dataAvailable && (data.size() < serialBufSize)) {
                dataAvailable = sp.waitForReadyRead(settings.idTimeout());
                if (dataAvailable) {
                    data.append(sp.read(oneByte));
                }
            }
            // Check to see if it contains data that looks like it is from a
            // sensor we support.
            // uADC
            if (data.contains('\n')) {
                if (settings.useUADC()) {
                    // Find newlines to try and get two packets.
                    quint16 newline1 = data.indexOf('\n') + 1;
                    quint16 newline2 = data.indexOf('\n', newline1 + 1) + 1;
                    // Get two packets.
                    QByteArray pkt1 = data.mid(newline1, dfti::uadcPktLen);
                    QByteArray pkt2 = data.mid(newline2, dfti::uadcPktLen);
                    // Make sure at least one verifies correctly.
                    if (dfti::validateUADCChecksum(pkt1) ||
                        dfti::validateUADCChecksum(pkt2)) {
                        if ((uadc != nullptr) && detectUADC) {
                            uadc->configureSerial(port.systemLocation());
                            detectUADC = false;
                            qDebug() << "uADC serial port set to"
                                     << port.systemLocation();
                        } else {
                            if (settings.debugSerial()) {
                                qDebug() << "Potentially found other uADC:"
                                         << port.systemLocation();
                            }
                        }
                        continue;
                    }
                }
            }
            // VN-200
            if (data.contains(vn200Header)) {
                if (settings.useVN200() && detectVN200) {
                    if (vn200 != nullptr) {
                        vn200->configureSerial(port.systemLocation());
                        detectVN200 = false;
                        qDebug() << "VN-200 serial port set to"
                                 << port.systemLocation();
                    }
                } else {
                    if (settings.debugSerial()) {
                        qDebug() << "Potentially found other VN-200:"
                                 << port.systemLocation();
                    }
                }
                continue;
            }
            // MAVLink stream
            if (data.contains(mavlinkSync)) {
                if (settings.useMavlink() && detectAP) {
                    if (ap != nullptr) {
                        ap->configureSerial(port.systemLocation());
                        detectAP = false;  // only detect the first A/P
                        qDebug() << "MAVLink/Pixhawk serial port set to"
                                 << port.systemLocation();
                    }
                } else {
                    if (settings.debugSerial()) {
                        qDebug() << "Potentially found other MAVLink/Pixhawk:"
                                 << port.systemLocation();
                    }
                }
                continue;
            }
            // We don't know.
            qDebug() << "Unknown serial device:" << port.systemLocation();
        } else {
            if (settings.debugSerial()) {
                qDebug() << "Failed to open candidate serial port"
                         << port.systemLocation();
                qDebug() << "Reason:" << sp.errorString();
            }
        }
    }
    if (sp.isOpen()) {
        sp.close();
    }
    return;
}


//! Main application function.
/*!
 *  Main function file for DFTI. Creates sensor objects and manages threads.
 *  \param argc Number of command line arguments.
 *  \param argv Array of command line arguments.
 */
int
main(int argc, char* argv[])
{
    // Set up application.
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(dfti::app_name);
    QCoreApplication::setApplicationVersion(dfti::app_version);

    // Register meta types.
    qRegisterMetaType<dfti::APData>("APData");
    qRegisterMetaType<dfti::uADCData>("uADCData");
    qRegisterMetaType<dfti::VN200Data>("VN200Data");

    // Set up command line parser.
    QCommandLineParser parser;
    parser.setApplicationDescription(
        QString("%1 -- Developmental Flight Test Instrumentation").arg(
            dfti::app_name));

    // Options
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(QCommandLineOption({"c", "config"},
        "Specify RC file.", "rc.ini"));
    parser.addOption(QCommandLineOption({"d", "debug-data"},
        "Display sensor data for debugging."));
    parser.addOption(QCommandLineOption({"r", "debug-rc"},
        "Display settings for debugging."));
    parser.addOption(QCommandLineOption({"s", "debug-serial"},
        "Display serial i/o for debugging."));
    parser.process(app);

    // Settings
    dfti::DebugMode debug = dfti::DebugMode::DEBUG_NONE;
    debug |= parser.isSet("debug-data") ?
        dfti::DebugMode::DEBUG_DATA : dfti::DebugMode::DEBUG_NONE;
    debug |= parser.isSet("debug-rc") ?
        dfti::DebugMode::DEBUG_RC : dfti::DebugMode::DEBUG_NONE;
    debug |= parser.isSet("debug-serial") ?
        dfti::DebugMode::DEBUG_SERIAL : dfti::DebugMode::DEBUG_NONE;

    // Create classes.
    dfti::Settings settings(parser.value("config"), debug);
    dfti::Logger *logger = new dfti::Logger(&settings);
    dfti::Autopilot *pixhawk = nullptr;
    dfti::uADC *uadc = nullptr;
    dfti::VN200 *vn200 = nullptr;

    // Instantiate sensor classes if sensors are available.
    if (settings.useMavlink()) {
        pixhawk = new dfti::Autopilot(&settings);
    }
    if (settings.useUADC()) {
        uadc = new dfti::uADC(&settings);
    }
    if (settings.useVN200()) {
        vn200 = new dfti::VN200(&settings);
    }

    // Set up serial ports.
    identifySerialPorts(settings, pixhawk, uadc, vn200);

    // Set up threads.
    QThread *loggingThread = new QThread();
    QThread *pixhawkThread = nullptr;
    QThread *uadcThread = nullptr;
    QThread *vn200Thread = nullptr;

    // Move objects to threads, and initialize sensor threads.
    logger->moveToThread(loggingThread);
    if (settings.useMavlink()) {
        pixhawkThread = new QThread();
        pixhawk->moveToThread(pixhawkThread);
    }
    if (settings.useUADC()) {
        uadcThread = new QThread();
        uadc->moveToThread(uadcThread);
    }
    if (settings.useVN200()) {
        vn200Thread = new QThread();
        vn200->moveToThread(vn200Thread);
    }

    // Connect everything.
    if (settings.useMavlink()) {
        logger->enableAutopilot(pixhawk);
    }
    if (settings.useUADC()) {
        logger->enableUADC(uadc);
    }
    if (settings.useVN200()) {
        logger->enableVN200(vn200);
    }
    if (settings.useMavlink()) {
        QObject::connect(pixhawkThread, &QThread::started, pixhawk,
            &dfti::Autopilot::threadStart);
    }
    if (settings.useUADC()) {
        QObject::connect(uadcThread, &QThread::started, uadc,
            &dfti::uADC::threadStart);
    }
    if (settings.useVN200()) {
        QObject::connect(vn200Thread, &QThread::started, vn200,
            &dfti::VN200::threadStart);
    }
    QObject::connect(loggingThread, &QThread::started, logger,
        &dfti::Logger::start);

    // Start the threads.
    if (settings.useMavlink()) {
        pixhawkThread->start();
    }
    if (settings.useUADC()) {
        uadcThread->start();
    }
    if (settings.useVN200()) {
        vn200Thread->start();
    }
    loggingThread->start();

    return app.exec();
}
