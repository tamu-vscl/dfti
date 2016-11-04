/*!
 *  \file main.cc
 *  \brief Aeroprobe Micro Air Data Computer test program.
 *  \author Joshua Harris
 *  \copyright ISC License
 */

// 3rd party
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QString>
// project
#include "uadc.hh"


// App info.
const QString app_name{"uadc_test"};
const QString app_version{"0.0.1"};


int
main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(app_name);
    QCoreApplication::setApplicationVersion(app_version);

    QCommandLineParser parser;
    parser.setApplicationDescription("uadc_test -- test uADC serial driver");
    // Positional Arguments
    parser.addPositionalArgument("port",
        QCoreApplication::translate("main",
            "Serial port name to connect to the uADC."));
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
    QString serial_port = args.first();

    // Instantiate the uADC class.
    dfti::uAirDataComputer uadc{parser.isSet("verbose")};

    // Start reading the sensor input.
    uadc.setSerialPort(serial_port);
    uadc.open();
    if (!uadc.isOpen()) {
        qDebug() << "failed to open serial port" << serial_port;
    }

    return app.exec();
}
