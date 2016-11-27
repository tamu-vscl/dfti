/*!
 *  \file settings.hh
 *  \brief DFTI settings manager interface.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#pragma once


// 3rd party
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QVector>
// project
#include "core/consts.hh"


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

    //! Should we prefer the MESSAGE_INTERVAL interface?
    /*!
     *  \remark MAVLink has deprecated the REQUEST_DATA_STREAM interface in
     *      favor of the new MESSAGE_INTERVAL interface, however APM does not
     *      yet (~Nov 2016) support this.
     */
    bool use_message_interval(void) const { return m_use_message_interval; };

    //! Return the desired MAVLink stream rate in Hz.
    quint32 stream_rate(void) const { return m_stream_rate; };

    //! Should we set the system time from GPS?
    bool set_system_time(void) const { return m_set_system_time; };

    //! Do we have a MAVLink-based autopilot?
    bool use_mavlink(void) const { return m_use_mavlink; };

    //! Do we have a Micro Air Data Computer?
    bool use_uadc(void) const { return m_use_uadc; };

    //! Do we have a VN-200 INS?
    bool use_vn200(void) const { return m_use_vn200; };

    //! Timeout for waitForReadyRead in identifySerialPorts.
    quint16 id_timeout(void) const { return m_id_timeout; };

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

    //! Prefer MESSAGE_INTERVAL to REQUEST_DATA_STREAM?
    bool m_use_message_interval{false};

    //! Stream rate in Hz for desired MAVLink parameters.
    quint32 m_stream_rate{10};

    //! Do we use a MAVLink-based autopilot?
    bool m_use_mavlink{false};

    //! Do we have a uADC?
    bool m_use_uadc{false};

    //! Do we have a VN-200?
    bool m_use_vn200{false};

    //! Should we set the system time?
    bool m_set_system_time{false};

    //! How long should we wait for a blocking serial read?
    /*!
     *  \remark This parameter controls how long the identifySerialPorts
     *      function waits for the readyRead signal.
     */
    quint16 m_id_timeout{2000};

};

};  // namespace dfti
