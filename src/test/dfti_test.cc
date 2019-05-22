/*!
 *  \file dfti_test.cc
 *  \brief DFTI component test program.
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


// 3rd party
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QPointer>
#include <QString>
// project
#include "autopilot/autopilot.hh"
#include "core/consts.hh"
#include "rio/rio.hh"
#include "uadc/uadc.hh"
#include "util/util.hh"
#include "vn200/vn200.hh"


//! App info.
const QString app_name{"dftitest"};


//! Main test application function.
/*!
 *  Main function file for DFTI test program. Creates sensor objects and
 *  manages threads.
 *  \param argc Number of command line arguments.
 *  \param argv Array of command line arguments.
 */
int
main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(app_name);
    QCoreApplication::setApplicationVersion(dfti::app_version);

    QCommandLineParser parser;
    parser.setApplicationDescription("dftitest -- test DFTI sensor drivers");
    // Positional Arguments
    parser.addPositionalArgument("sensor",
        QCoreApplication::translate("main",
            "Sensor to test, one of (ap|rio|uadc|vn200)."));
    parser.addPositionalArgument("port",
        QCoreApplication::translate("main",
            "Serial port name to connect to."));
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

    // Variables to hold positional arguments.
    QString sensor_name{"ap"};
    QString serial_port{"/dev/ttyUSB0"};

    // Get positional arguments.
    QStringList validArgs;
    validArgs << "ap" << "rio" << "uadc" << "vn200";
    QStringList args = parser.positionalArguments();
    if (!args.isEmpty()) {
        sensor_name = args.first();
        if (!validArgs.contains(sensor_name)) {
            qWarning() << "Sensor name must be one of {ap, rio, uadc, vn200}";
            exit(-1);
        }
        if (args.size() > 1) {
            serial_port = args.value(1);
        } else {
            qWarning() << "Must provide sensor name and serial port name.";
            exit(-1);
        }
    }

    // Settings
    dfti::DebugMode debug = dfti::DebugMode::DEBUG_NONE;
    debug |= parser.isSet("debug-data") ?
        dfti::DebugMode::DEBUG_DATA : dfti::DebugMode::DEBUG_NONE;
    debug |= parser.isSet("debug-rc") ?
        dfti::DebugMode::DEBUG_RC : dfti::DebugMode::DEBUG_NONE;
    debug |= parser.isSet("debug-serial") ?
        dfti::DebugMode::DEBUG_SERIAL : dfti::DebugMode::DEBUG_NONE;
    dfti::Settings settings(parser.value("config"), debug);

    // Create sensor object pointers.
    QPointer<dfti::Autopilot> pixhawk = nullptr;
    QPointer<dfti::RIO> rio = nullptr;
    QPointer<dfti::uADC> uadc = nullptr;
    QPointer<dfti::VN200> vn200 = nullptr;

    // Instantiate the chosen sensor based on the argument name.
    if (sensor_name == "ap") {
        pixhawk = new dfti::Autopilot(&settings);
        pixhawk->configureSerial(serial_port);
        pixhawk->threadStart();
        if (!pixhawk->isOpen()) {
            qDebug() << "failed to open autopilot serial port" << serial_port;
            exit(-1);
        }
    } else if (sensor_name == "rio") {
        rio = new dfti::RIO(&settings);
        rio->configureSerial(serial_port);
        rio->threadStart();
        if (!rio->isOpen()) {
            qDebug() << "failed to open RIO serial port" << serial_port;
            exit(-1);
        }
    } else if (sensor_name == "uadc") {
        uadc = new dfti::uADC(&settings);
        uadc->configureSerial(serial_port);
        uadc->threadStart();
        if (!uadc->isOpen()) {
            qDebug() << "failed to open uADC serial port" << serial_port;
            exit(-1);
        }
    } else if (sensor_name == "vn200") {
        vn200 = new dfti::VN200(&settings);
        vn200->configureSerial(serial_port);
        vn200->threadStart();
        if (!vn200->isOpen()) {
            qDebug() << "failed to open VN-200 serial port" << serial_port;
            exit(-1);
        }
    } else {
        qWarning() << "[WARN] Invalid sensor type";
        exit(-1);
    }

    return app.exec();
}
