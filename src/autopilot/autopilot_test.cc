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
    parser.setApplicationDescription("ap_test -- test MAVlinl serial driver");
    // Positional Arguments
    parser.addPositionalArgument("port",
        QCoreApplication::translate("main",
            "Serial port name to connect to the autopilot."));
    // Options
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions({
        {
            {"V", "verbose"},
            QCoreApplication::translate("main",
                "Request verbose console output.")
        }
    });
    parser.process(app);
    // Get serial port name.
    QStringList args = parser.positionalArguments();
    QString serial_port{"/dev/ttyUSB0"};
    if (!args.isEmpty()) {
        serial_port = args.first();
    }

    // Instantiate the VN200 class.
    dfti::Autopilot pixhawk{parser.isSet("verbose")};

    // Start reading the sensor input.
    pixhawk.setSerialPort(serial_port);
    pixhawk.open();
    if (!pixhawk.isOpen()) {
        qDebug() << "failed to open serial port" << serial_port;
        exit(-1);
    }

    return app.exec();
}
