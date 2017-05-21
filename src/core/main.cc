/*!
 *  \file main.cc
 *  \brief DFTI main program.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016-2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license BSD 2-Clause License
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
#include "rio/rio.hh"
#include "server/server.hh"
#include "uadc/uadc.hh"
#include "util/util.hh"
#include "vn200/vn200.hh"



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
    qRegisterMetaType<dfti::RIOData>("RIOData");
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
    dfti::Server *server = nullptr;
    dfti::Autopilot *pixhawk = nullptr;
    dfti::RIO *rio = nullptr;
    dfti::uADC *uadc = nullptr;
    dfti::VN200 *vn200 = nullptr;

    // Instantiate server if enabled.
    if (settings.serverEnabled()) {
        server = new dfti::Server(&settings);
    }

    // Instantiate sensor classes if sensors are available.
    if (settings.useMavlink()) {
        pixhawk = new dfti::Autopilot(&settings);
        pixhawk->configureSerial(settings.autopilotSerialPort());
    }
    if (settings.useRIO()) {
        rio = new dfti::RIO(&settings);
        rio->configureSerial(settings.rioSerialPort());
    }
    if (settings.useUADC()) {
        uadc = new dfti::uADC(&settings);
        uadc->configureSerial(settings.uADCSerialPort());
    }
    if (settings.useVN200()) {
        vn200 = new dfti::VN200(&settings);
        vn200->configureSerial(settings.vn200SerialPort());
    }

    // Set up threads.
    QThread *loggingThread = new QThread();
    QThread *serverThread = nullptr;
    QThread *pixhawkThread = nullptr;
    QThread *rioThread = nullptr;
    QThread *uadcThread = nullptr;
    QThread *vn200Thread = nullptr;

    // Move objects to threads, and initialize sensor threads.
    logger->moveToThread(loggingThread);
    if (settings.serverEnabled()) {
        serverThread = new QThread();
        server->moveToThread(serverThread);
    }
    if (settings.useMavlink()) {
        pixhawkThread = new QThread();
        pixhawk->moveToThread(pixhawkThread);
    }
    if (settings.useRIO()) {
        rioThread = new QThread();
        rio->moveToThread(rioThread);
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
        QObject::connect(pixhawkThread, &QThread::started, pixhawk,
            &dfti::Autopilot::threadStart);
    }
    if (settings.useRIO()) {
        logger->enableRIO(rio);
        server->enableRIO(rio);
        QObject::connect(rioThread, &QThread::started, rio,
            &dfti::RIO::threadStart);
    }
    if (settings.useUADC()) {
        logger->enableUADC(uadc);
        server->enableUADC(uadc);
        QObject::connect(uadcThread, &QThread::started, uadc,
            &dfti::uADC::threadStart);
    }
    if (settings.useVN200()) {
        logger->enableVN200(vn200);
        server->enableVN200(vn200);
        QObject::connect(vn200Thread, &QThread::started, vn200,
            &dfti::VN200::threadStart);
    }
    QObject::connect(loggingThread, &QThread::started, logger,
        &dfti::Logger::start);
    QObject::connect(serverThread, &QThread::started, server,
        &dfti::Server::start);

    // Start the threads.
    if (settings.useMavlink()) {
        pixhawkThread->start();
    }
    if (settings.useRIO()) {
        rioThread->start();
    }
    if (settings.useUADC()) {
        uadcThread->start();
    }
    if (settings.useVN200()) {
        vn200Thread->start();
    }
    loggingThread->start();
    if (settings.serverEnabled()) {
        serverThread->start();
    }

    return app.exec();
}
