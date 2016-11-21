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
#include "vn200/vn200.hh"


int
main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(dfti::app_name);
    QCoreApplication::setApplicationVersion(QString("%1.%2.%3").arg(
        dfti_MAJOR_VERSION, dfti_MINOR_VERSION, dfti_PATCH_VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription(
        QString("%1 -- Developmental Flight Test Instrumenation").arg(
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

    dfti::Settings settings(parser.value("config"), debug);

    // Register meta types.
    qRegisterMetaType<dfti::APData>("APData");
    qRegisterMetaType<dfti::uADCData>("uADCData");
    qRegisterMetaType<dfti::VN200Data>("VN200Data");

    // Create the various class instances.
    dfti::Logger *logger = new dfti::Logger(&settings);
    dfti::Autopilot *pixhawk = new dfti::Autopilot(&settings);
    pixhawk->configureSerial("/dev/ttyACM0");
    // dfti::uADC *uadc new dfti::uADC(&settings);
    dfti::VN200 *vn200 = new dfti::VN200(&settings);
    vn200->configureSerial("/dev/ttyUSB0");

    // Set up threads.
    QThread *loggingThread = new QThread();
    logger->moveToThread(loggingThread);
    QThread *pixhawkThread = new QThread();
    pixhawk->moveToThread(pixhawkThread);
    QThread *vn200Thread = new QThread();
    vn200->moveToThread(vn200Thread);

    // Connect everything.
    logger->enableAutopilot(pixhawk);
    logger->enableVN200(vn200);
    QObject::connect(pixhawkThread, &QThread::started, pixhawk,
        &dfti::Autopilot::threadStart);
    QObject::connect(vn200Thread, &QThread::started, vn200,
        &dfti::VN200::threadStart);
    QObject::connect(loggingThread, &QThread::started, logger,
        &dfti::Logger::start);

    // Start the threads.
    pixhawkThread->start();
    vn200Thread->start();
    loggingThread->start();

    return app.exec();
}
