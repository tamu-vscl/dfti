/*!
 *  \file settings.hh
 *  \brief DFTI settings manager interface.
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
#pragma once


// 3rd party
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QHostAddress>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QVector>
// project
#include "core/consts.hh"
#include "util/util.hh"


namespace dfti {


//! Settings manager.
class Settings : public QObject
{
    Q_OBJECT;

public:
    //! Ctor.
    Settings(QString _rcfile, DebugMode _debug);

    //! Returns true if settings debug messages are enabled.
    bool debugRC() const {
        return check(m_debug & DebugMode::DEBUG_RC);
    };

    //! Returns true if sensor data debug messages are enabled.
    bool debugData() const {
        return check(m_debug & DebugMode::DEBUG_DATA);
    };

    //! Returns true if serial i/o debug messages are enabled.
    bool debugSerial() const {
        return check(m_debug & DebugMode::DEBUG_SERIAL);
    };

    //! Load a settings file.
    /*!
     *  \param _fn Filename for settings file.
     */
    void loadRCFile(QString _fn);

    //! Return the log sampling time in ms.
    float logRateMs(void) const { return m_logRateMs; };

    //! Return the log flush timer period in ms.
    float flushRateMs(void) const { return m_flushRateMs; };

    //! Return the server sampling time in ms.
    float sendRateMs(void) const { return m_sendRateMs; };

    //! Return the server status.
    bool serverEnabled(void) const { return m_serverEnabled; };

    //! Return the server address.
    QHostAddress serverAddress(void) const { return m_serverAddress; };

    //! Return the server port.
    quint16 serverPort(void) const { return m_serverPort; };

    //! Should we prefer the MESSAGE_INTERVAL interface?
    /*!
     *  \remark MAVLink has deprecated the REQUEST_DATA_STREAM interface in
     *      favor of the new MESSAGE_INTERVAL interface, however APM does not
     *      yet (~Nov 2016) support this.
     */
    bool useMessageInterval(void) const { return m_useMessageInterval; };

    //! Return the desired MAVLink stream rate in Hz.
    quint32 streamRate(void) const { return m_streamRate; };

    //! Should we set the system time from GPS?
    bool setSystemTime(void) const { return m_setSystemTime; };

    //! Do we have RIO logging?
    bool useRIO(void) const { return m_useRIO; };

    //! Do we have a MAVLink-based autopilot?
    bool useMavlink(void) const { return m_useMavlink; };

    //! Do we have a Micro Air Data Computer?
    bool useUADC(void) const { return m_useUADC; };

    //! Do we have a VN-200 INS?
    bool useVN200(void) const { return m_useVN200; };

    //! Should we wait for the RIO values before logging?
    bool waitForRIO(void) const { return m_waitForRIO; };

    //! Should we wait for the MAVLink init message before logging?
    bool waitForMavInit(void) const { return m_waitForMavInit; };

    //! Should we wait for VN200 GPS before logging?
    bool waitForVN200GPS(void) const { return m_waitForVN200GPS; };

    //! Should we wait for all sensors to get data before writing?
    bool waitForAllSensors(void) const { return m_waitForAllSensors; };

    //! Should we wait for a data update to write to the log?
    bool waitForUpdate(void) const { return m_waitForUpdate; };

    //! Overridden Autopilot serial port.
    QString autopilotSerialPort(void) const { return m_autopilotSerialPort; };

    //! Overridden RIO serial port.
    QString rioSerialPort(void) const { return m_rioSerialPort; };

    //! Overridden uADC serial port.
    QString uADCSerialPort(void) const { return m_uADCSerialPort; };

    //! Overridden VN-200 serial port.
    QString vn200SerialPort(void) const { return m_vn200SerialPort; };

    //! Overridden Autopilot baud rate.
    quint32 autopilotBaudRate(void) const { return m_autopilotBaudRate; };

    //! Overridden RIO baud rate.
    quint32 rioBaudRate(void) const { return m_rioBaudRate; };

    //! Overridden uADC baud rate.
    quint32 uADCBaudRate(void) const { return m_uADCBaudRate; };

    //! Overridden VN-200 baud rate.
    quint32 vn200BaudRate(void) const { return m_vn200BaudRate; };

private:
    //! Settings file name.
    QString m_rcfile;

    //! User settings file name.
    QString m_userRC;

    //! System-wide settings file name.
    const QString m_sysRC{"/etc/dftirc"};

    //! Settings object.
    QSettings* m_settings{nullptr};

    //! Debug settings.
    DebugMode m_debug{DebugMode::DEBUG_NONE};

    //! Log sample time in ms.
    float m_logRateMs{10};

    //! Flush timer in ms.
    float m_flushRateMs{1e4};

    //! Server status.
    bool m_serverEnabled{false};

    //! Server sample time in ms.
    float m_sendRateMs{20};

    //! Server address.
    QHostAddress m_serverAddress{QHostAddress::LocalHost};

    //! Server port.
    quint16 m_serverPort{2701};

    //! Prefer MESSAGE_INTERVAL to REQUEST_DATA_STREAM?
    bool m_useMessageInterval{false};

    //! Stream rate in Hz for desired MAVLink parameters.
    quint32 m_streamRate{10};

    //! Do we use a MAVLink-based autopilot?
    bool m_useMavlink{false};

    //! Do we have a RIO?
    bool m_useRIO{false};

    //! Do we have a uADC?
    bool m_useUADC{false};

    //! Do we have a VN-200?
    bool m_useVN200{false};

    //! Should we set the system time?
    bool m_setSystemTime{false};

    //! Should we wait for the MAVLink init message before logging?
    bool m_waitForMavInit{false};

    //! Should we wait for RIO values before logging?
    bool m_waitForRIO{false};

    //! Should we wait for VN200 GPS before logging?
    bool m_waitForVN200GPS{false};

    //! Should we wait for all sensors to get data before writing?
    bool m_waitForAllSensors{false};

    //! Should we wait for a data update to write to the log?
    bool m_waitForUpdate{true};

    //! Overridden Autopilot serial port.
    QString m_autopilotSerialPort{""};

    //! Overridden RIO serial port.
    QString m_rioSerialPort{""};

    //! Overridden uADC serial port.
    QString m_uADCSerialPort{""};

    //! Overridden VN-200 serial port.
    QString m_vn200SerialPort{""};

    //! Overridden Autopilot baud rate.
    quint32 m_autopilotBaudRate{0};

    //! Overridden RIO baud rate.
    quint32 m_rioBaudRate{0};

    //! Overridden uADC baud rate.
    quint32 m_uADCBaudRate{0};

    //! Overridden VN-200 baud rate.
    quint32 m_vn200BaudRate{0};
};

};  // namespace dfti
