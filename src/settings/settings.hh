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

private:
    QString m_rcfile;
    QString m_userRC;
    QString m_sysRC{"/etc/dftirc"};

    //! Settings object.
    QSettings* m_settings = nullptr;

    //! Debug settings.
    DebugMode m_debug;

    //! Prefer MESSAGE_INTERVAL to REQUEST_DATA_STREAM?
    bool m_use_message_interval{false};

    //! Stream rate in Hz for desired MAVLink parameters.
    quint32 m_stream_rate{10};
};

};  // namespace dfti
