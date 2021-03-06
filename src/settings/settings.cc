/*!
 *  \file settings.cc
 *  \brief DFTI settings manager implementation.
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
#include "settings.hh"


namespace dfti {


// ----------------------------------------------------------------------------
//  Constructors/destructors
// ----------------------------------------------------------------------------
Settings::Settings(QString _rcfile, DebugMode _debug) : m_debug(_debug)
{
    m_userRC = QDir::home().absolutePath() + "/.config/dfti/rc.ini";

    if (debugRC()) {
        qDebug() << "Command line rc file path:" << _rcfile;
        qDebug() << "User rc file path:" << m_userRC;
        qDebug() << "System rc file path:" << m_sysRC;
    }

    if (_rcfile != "" && QFile::exists(_rcfile)) {
        m_rcfile = _rcfile;
        if (debugRC()) {
            qDebug() << "Using configuration file from command line";
        }
    } else if (QFile::exists(m_userRC)) {
        m_rcfile = m_userRC;
        if (debugRC()) {
            qDebug() << "Using configuration file" << m_userRC << ".";
        }
    } else if (QFile::exists(m_sysRC)) {
        m_rcfile = m_sysRC;
        if (debugRC()) {
            qDebug() << "Using configuration file" << m_sysRC << ".";
        }
    } else {
        qWarning() << "No valid configuration file found. Exiting.";
        exit(1);
    }
    loadRCFile(m_rcfile);
    if (debugRC())
        qDebug() << "Finished settings initialization.";
    return;
}

// ----------------------------------------------------------------------------
//  Public functions
// ----------------------------------------------------------------------------
void
Settings::loadRCFile(QString _fn)
{
    if (m_settings) {
        delete m_settings;
        m_settings = nullptr;
    }
    m_settings = new QSettings(_fn, QSettings::IniFormat);

    // Display child keys and groups. This makes arrays work for some reason...
    if (debugRC()) {
        qDebug() << "Child Keys/Groups:" << m_settings->childKeys() << "/"
                 << m_settings->childGroups();
    } else {
        m_settings->childKeys();
        m_settings->childGroups();
    }

    if (debugRC()) {
        qDebug() << "Created QSettings from file" << _fn;
    }

    // DFTI parameters.
    m_settings->beginGroup("dfti");
    m_setSystemTime = m_settings->value("set_system_time", false).toBool();
    m_useMavlink = m_settings->value("use_mavlink", false).toBool();
    m_useRIO = m_settings->value("use_rio", false).toBool();
    m_useUADC = m_settings->value("use_uadc", false).toBool();
    m_useVN200 = m_settings->value("use_vn200", false).toBool();
    quint16 logRateHz = m_settings->value("log_rate_hz", 100).toInt();
    m_logRateMs = hzToMsec(logRateHz);
    quint16 flushTimeSec = m_settings->value("flush_time_sec", 10).toInt();
    m_flushRateMs = secToMsec(flushTimeSec);
    m_waitForAllSensors = m_settings->value("wait_for_all_sensors",
        false).toBool();
    m_waitForUpdate = m_settings->value("wait_for_update", true).toBool();
    m_settings->endGroup();
    if (debugRC()) {
        qDebug() << "Loaded [dfti] settings group:";
        qDebug() << "\tlog_rate_hz:           " << logRateHz;
        qDebug() << "\tflush_time_sec:        " << flushTimeSec;
        qDebug() << "\tset_system_time:       " << m_setSystemTime;
        qDebug() << "\tuse_mavlink:           " << m_useMavlink;
        qDebug() << "\tuse_rio:               " << m_useRIO;
        qDebug() << "\tuse_uadc:              " << m_useUADC;
        qDebug() << "\tuse_vn200:             " << m_useVN200;
        qDebug() << "\twait_for_all_sensors:  " << m_waitForAllSensors;
        qDebug() << "\twait_for_update:       " << m_waitForUpdate;
    }

    // Server parameters.
    m_settings->beginGroup("server");
    m_serverEnabled = m_settings->value("enabled", false).toBool();
    m_serverAddress = QHostAddress(m_settings->value("address",
        "127.0.0.1").toString());
    m_serverPort = static_cast<quint16>(m_settings->value("port",
        2701).toInt());
    // Get the server rate and make it at most 1/2 of the log rate.
    quint8 serverRateHz = m_settings->value("rate_hz", 50).toInt();
    if (2 * serverRateHz > logRateHz) {
        serverRateHz = static_cast<quint8>(0.5 * logRateHz);
    }
    m_sendRateMs = hzToMsec(serverRateHz);
    m_settings->endGroup();
    if (debugRC()) {
        qDebug() << "Loaded [server] settings group:";
        qDebug() << "\tenabled:              " << m_serverEnabled;
        qDebug() << "\taddress:              " << m_serverAddress;
        qDebug() << "\tport:                 " << m_serverPort;
        qDebug() << "\trate_hz:              " << serverRateHz;
    }

    // MAVLink parameters.
    m_settings->beginGroup("mavlink");
    m_autopilotBaudRate = m_settings->value("baud_rate", 0).toInt();
    m_autopilotSerialPort = m_settings->value("serial_port", "").toString();
    m_streamRate = m_settings->value("stream_rate", 10).toInt();
    m_useMessageInterval = m_settings->value("use_message_interval",
        false).toBool();
    m_waitForMavInit = m_settings->value("wait_for_init", false).toBool();
    m_settings->endGroup();
    if (debugRC()) {
        qDebug() << "Loaded [mavlink] settings group:";
        qDebug() << "\tbaud_rate:             " << m_autopilotBaudRate;
        qDebug() << "\tserial_port:           " << m_autopilotSerialPort;
        qDebug() << "\tstream_rate:           " << m_streamRate;
        qDebug() << "\tuse_message_interval:  " << m_useMessageInterval;
        qDebug() << "\twait_for_init:         " << m_waitForMavInit;
    }

    // Remote I/O parameters.
    m_settings->beginGroup("rio");
    m_rioBaudRate = m_settings->value("baud_rate", 0).toInt();
    m_rioSerialPort = m_settings->value("serial_port", "").toString();
    m_settings->endGroup();
    if (debugRC()) {
        qDebug() << "Loaded [rio] settings group:";
        qDebug() << "\tbaud_rate:             " << m_rioBaudRate;
        qDebug() << "\tserial_port:           " << m_rioSerialPort;
    }

    // uADC parameters.
    m_settings->beginGroup("uadc");
    m_uADCBaudRate = m_settings->value("baud_rate", 0).toInt();
    m_uADCSerialPort = m_settings->value("serial_port", "").toString();
    m_settings->endGroup();
    if (debugRC()) {
        qDebug() << "Loaded [uadc] settings group:";
        qDebug() << "\tbaud_rate:             " << m_uADCBaudRate;
        qDebug() << "\tserial_port:           " << m_uADCSerialPort;
    }

    // VN-200 parameters.
    m_settings->beginGroup("vn200");
    m_vn200BaudRate = m_settings->value("baud_rate", 0).toInt();
    m_vn200SerialPort = m_settings->value("serial_port", "").toString();
    m_waitForVN200GPS = m_settings->value("wait_for_gps", false).toBool();
    m_settings->endGroup();
    if (debugRC()) {
        qDebug() << "Loaded [vn200] settings group:";
        qDebug() << "\tbaud_rate:             " << m_vn200BaudRate;
        qDebug() << "\tserial_port:           " << m_vn200SerialPort;
        qDebug() << "\twait_for_gps:          " << m_waitForVN200GPS;
    }

    return;
}

// ----------------------------------------------------------------------------
// Public Slots
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  Private functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  Functions
// ----------------------------------------------------------------------------


};  // namespace dfti
