/*!
 *  \file vn200_test.cc
 *  \brief VectorNav VN-200 Inertial Navigation System  test program.
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
#include "vn200.hh"


// App info.
const QString app_name{"vn200_test"};
const QString app_version{"0.0.1"};


int
main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(app_name);
    QCoreApplication::setApplicationVersion(app_version);

    QCommandLineParser parser;
    parser.setApplicationDescription("vn200_test -- test VN-200 serial driver");
    // Positional Arguments
    parser.addPositionalArgument("port",
        QCoreApplication::translate("main",
            "Serial port name to connect to the VN-200."));
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
    dfti::VN200 vn200{parser.isSet("verbose")};

    // Start reading the sensor input.
    vn200.setSerialPort(serial_port);
    vn200.open();
    if (!vn200.isOpen()) {
        qDebug() << "failed to open serial port" << serial_port;
        exit(-1);
    }

    return app.exec();
}
