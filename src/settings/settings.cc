/*!
 *  \file settings.cc
 *  \brief DFTI settings manager implementation.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
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
