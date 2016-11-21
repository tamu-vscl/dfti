/*!
 *  \file atopilot_test.cc
 *  \brief MAVLink-based autopilot test program.
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
#include <QString>
// project
#include "autopilot.hh"
#include "core/consts.hh"
#include "settings/settings.hh"


// App info.
const QString app_name{"ap_test"};
const QString app_version{"0.0.1"};


int
main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(app_name);
    QCoreApplication::setApplicationVersion(app_version);

    QCommandLineParser parser;
    parser.setApplicationDescription("ap_test -- test MAVlink serial driver");
    // Positional Arguments
    parser.addPositionalArgument("port",
        QCoreApplication::translate("main",
            "Serial port name to connect to the autopilot."));
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
    // Get serial port name.
    QStringList args = parser.positionalArguments();
    QString serial_port{"/dev/ttyUSB0"};
    if (!args.isEmpty()) {
        serial_port = args.first();
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

    // Instantiate the VN200 class.
    dfti::Autopilot pixhawk{&settings};

    // Start reading the sensor input.
    pixhawk.configureSerial(serial_port);
    pixhawk.init();
    pixhawk.open();
    if (!pixhawk.isOpen()) {
        qDebug() << "failed to open serial port" << serial_port;
        exit(-1);
    }

    return app.exec();
}
