/*!
 *  \file logger.cc
 *  \brief DFTI logging interface implementation.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016-2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#include "logger.hh"


namespace dfti {


// ----------------------------------------------------------------------------
//  Constructors/destructors
// ----------------------------------------------------------------------------
Logger::Logger(Settings *_settings, QObject* _parent)
: settings(_settings), QObject(_parent)
{
    // Get ISO date timestamp.
    QDateTime local(QDateTime::currentDateTime());
    QDateTime UTC(local.toTimeSpec(Qt::UTC));
    // QDateTime format doesn't support formatters without separators, so use
    // a dummy value 'R' and then replace it.
    timestamp = UTC.toString("yyyy'R'MM'R'dd'T'HH'R'mm");
    timestamp.replace(QString("R"), QString(""));
}


Logger::~Logger()
{
    if (writeTimer != nullptr) {
        delete writeTimer;
        writeTimer = nullptr;
    }
    if (flushTimer != nullptr) {
        delete flushTimer;
        flushTimer = nullptr;
    }
}

// ----------------------------------------------------------------------------
//  Public functions
// ----------------------------------------------------------------------------
void
Logger::enableAutopilot(Autopilot *ap)
{
    haveAP = true;
    connect(ap, &Autopilot::measurementUpdate, this, &Logger::getAPData);
    openLogFile(apLogFile, apLogFileOpen, "autopilot", timestamp);
}


void
Logger::enableRIO(RIO *rio)
{
    haveRIO = true;
    connect(rio, &RIO::measurementUpdate, this, &Logger::getRIOData);
    openLogFile(rioLogFile, rioLogFileOpen, "rio", timestamp);
}


void
Logger::enableUADC(uADC *adc)
{
    haveUADC = true;
    connect(adc, &uADC::measurementUpdate, this, &Logger::getUADCData);
    openLogFile(uADCLogFile, uADCLogFileOpen, "uadc", timestamp);
}


void
Logger::enableVN200(VN200 *ins)
{
    haveVN200 = true;
    connect(ins, &VN200::measurementUpdate, this, &Logger::getVN200Data);
    connect(ins, &VN200::gpsAvailable, this, &Logger::gpsAvailable);
    openLogFile(vn200LogFile, vn200LogFileOpen, "vn200", timestamp);
}


void
Logger::start(void)
{
    writeTimer = new QTimer(this);
    connect(writeTimer, &QTimer::timeout, this, &Logger::writeData);
    flushTimer = new QTimer(this);
    connect(flushTimer, &QTimer::timeout, this, &Logger::flush);
    writeTimer->start(settings->logRateMs());
    flushTimer->start(settings->flushRateMs());
}

// ----------------------------------------------------------------------------
// Public Slots
// ----------------------------------------------------------------------------
void
Logger::flush(void)
{
    if (apLogFileOpen) {
        apLogFile.flush();
    }
    if (rioLogFileOpen) {
        rioLogFile.flush();
    }
    if (uADCLogFileOpen) {
        uADCLogFile.flush();
    }
    if (vn200LogFileOpen) {
        vn200LogFile.flush();
    }
}


void
Logger::getAPData(APData data)
{
    rcInTime = data.rcInTime;
    rcIn1 = data.rcIn1;
    rcIn2 = data.rcIn2;
    rcIn3 = data.rcIn3;
    rcIn4 = data.rcIn4;
    rcIn5 = data.rcIn5;
    rcIn6 = data.rcIn6;
    rcIn7 = data.rcIn7;
    rcIn8 = data.rcIn8;
    rcOutTime = data.rcOutTime;
    rcOut1 = data.rcOut1;
    rcOut2 = data.rcOut2;
    rcOut3 = data.rcOut3;
    rcOut4 = data.rcOut4;
    rcOut5 = data.rcOut5;
    rcOut6 = data.rcOut6;
    rcOut7 = data.rcOut7;
    rcOut8 = data.rcOut8;
    newAPData = true;
    if (settings->debugSerial()) {
        qDebug() << "Logger::getAPData";
    }
}


void
Logger::getRIOData(RIOData data)
{
    rioData = data.values;
    newRIOData = true;
    if (settings->debugSerial()) {
        qDebug() << "Logger::getRIOData";
    }
}


void
Logger::getUADCData(uADCData data)
{
    uadcId = data.id;
    iasMps = data.iasMps;
    aoaDeg = data.aoaDeg;
    aosDeg = data.aosDeg;
    altM = data.altM;
    ptPa = data.ptPa;
    psPa = data.psPa;
    newUADCData = true;
    if (settings->debugSerial()) {
        qDebug() << "Logger::getuADCData";
    }
}


void
Logger::getVN200Data(VN200Data data)
{
    gpsTimeNs = data.gpsTimeNs;
    for (quint8 i = 0; i < 3; ++i) {
        eulerDeg[i] = data.eulerDeg[i];
        quaternion[i] = data.quaternion[i];
        angularRatesRPS[i] = data.angularRatesRPS[i];
        posDegDegM[i] = data.posDegDegM[i];
        velNedMps[i] = data.velNedMps[i];
        accelMps2[i] = data.accelMps2[i];
    }
    quaternion[3] = data.quaternion[3];
    newVN200Data = true;
    if (settings->debugSerial()) {
        qDebug() << "Logger::getVN200Data";
    }
}


void
Logger::gpsAvailable(bool flag)
{
    haveGPS = flag;
    if (settings->setSystemTime()) {
        // Make sure we haven't already set the system time and that the GPS
        // time value actually is GPS time. (Current GPS timestamp in
        // nanoseconds should always be greater than 1e18.)
        if ((!setSystemTime) && (gpsTimeNs > 1e18)) {
            // See http://unix.stackexchange.com/a/84138
            QString program{"date"};
            QStringList arguments;
            arguments << "+%s" << "-s"
                      << QString("@%1").arg(gpsToUnixSec(gpsTimeNs));
            QProcess process(this);
            process.start(program, arguments);
            // Assume we set the system time.
            setSystemTime = true;
            // Wait at most 2 seconds for the process to finish.
            if (process.waitForFinished(2000)) {
                if ((process.exitStatus() == QProcess::NormalExit) &&
                    (process.exitCode() == 0)) {
                    if (settings->debugRC()) {
                        qDebug() << "Set system time.";
                    }
                } else {
                    qWarning() << "[WARN ]  Failed to set system time.";
                    // If we detect we failed to set the system time, reset the
                    // flag.
                    setSystemTime = false;
                }
            }
        }
    }
}


void
Logger::writeData(void)
{
    QTextStream apOut(&apLogFile);
    QTextStream rioOut(&rioLogFile);
    QTextStream uADCOut(&uADCLogFile);
    QTextStream vn200Out(&vn200LogFile);
    apOut.setRealNumberNotation(QTextStream::FixedNotation);
    rioOut.setRealNumberNotation(QTextStream::FixedNotation);
    uADCOut.setRealNumberNotation(QTextStream::FixedNotation);
    vn200Out.setRealNumberNotation(QTextStream::FixedNotation);

    if (firstWrite) {
        // VN-200 data.
        if (logVN200()) {
            vn200Out << "unix_time" << delim
                     << "gps_time_ns" << delim
                     << "psi_deg" << delim
                     << "theta_deg" << delim
                     << "phi_deg" << delim
                     << "quat_w" << delim
                     << "quat_x" << delim
                     << "quat_y" << delim
                     << "quat_z" << delim
                     << "p_rps" << delim
                     << "q_rps" << delim
                     << "r_rps" << delim
                     << "lat_deg" << delim
                     << "lon_deg" << delim
                     << "alt_m" << delim
                     << "Vx_mps" << delim
                     << "Vy_mps" << delim
                     << "Vz_mps" << delim
                     << "Ax_mps2" << delim
                     << "Ay_mps2" << delim
                     << "Az_mps2" << '\n';
          firstWrite = false;
        }
        // RIO data.
        if (logRIO()){
          rioOut << "unix_time";
          for (quint8 i = 0; i < rioData.size(); ++i) {
            rioOut << delim << "rio_value_" << i;
          }
          rioOut << '\n';
          firstWrite = false;
        }
        // Air data system data.
        if (logUADC()) {
            uADCOut << "unix_time" << delim
                    << "uadc_id" << delim
                    << "ias_mps" << delim
                    << "aoa_deg" << delim
                    << "aos_deg" << delim
                    << "alt_m" << delim
                    << "pt_pa" << delim
                    << "ps_pa" << '\n';
          firstWrite = false;
        }
        // Autopilot data.
        if (logAP()) {
            apOut << "unix_time" << delim
                  << "rc_in_time" << delim
                  << "rc_in_1_pwm" << delim
                  << "rc_in_2_pwm" << delim
                  << "rc_in_3_pwm" << delim
                  << "rc_in_4_pwm" << delim
                  << "rc_in_5_pwm" << delim
                  << "rc_in_6_pwm" << delim
                  << "rc_in_7_pwm" << delim
                  << "rc_in_8_pwm" << delim
                  << "rc_out_time" << delim
                  << "rc_out_1_pwm" << delim
                  << "rc_out_2_pwm" << delim
                  << "rc_out_3_pwm" << delim
                  << "rc_out_4_pwm" << delim
                  << "rc_out_5_pwm" << delim
                  << "rc_out_6_pwm" << delim
                  << "rc_out_7_pwm" << delim
                  << "rc_out_8_pwm" << '\n';
          firstWrite = false;
        }
    }

    // System time in microseconds.
    quint64 ts = getTimeUsec();

    // VN-200 data.
    if (logVN200()) {
        vn200Out.setRealNumberPrecision(7);  // float
        vn200Out << ts << delim
                 << gpsTimeNs << delim
                 << eulerDeg[0] << delim
                 << eulerDeg[1] << delim
                 << eulerDeg[2] << delim
                 << quaternion[0] << delim
                 << quaternion[1] << delim
                 << quaternion[2] << delim
                 << quaternion[3] << delim
                 << angularRatesRPS[0] << delim
                 << angularRatesRPS[1] << delim
                 << angularRatesRPS[2] << delim;
        vn200Out.setRealNumberPrecision(15);  // double
        vn200Out << posDegDegM[0] << delim
                 << posDegDegM[1] << delim
                 << posDegDegM[2] << delim;
        vn200Out.setRealNumberPrecision(7);  // float
        vn200Out << velNedMps[0] << delim
                 << velNedMps[1] << delim
                 << velNedMps[2] << delim
                 << accelMps2[0] << delim
                 << accelMps2[1] << delim
                 << accelMps2[2] << '\n';
        newVN200Data = false;
    }

    // RIO data.
    if (logRIO()) {
      rioOut << ts;
      for (auto value : rioData) {
        rioOut << delim << value;
      }
      rioOut << '\n';
      newRIOData = false;
    }

    // Air data system data.
    if (logUADC()) {
        // We get two decimal places from the uADC...
        uADCOut.setRealNumberPrecision(2);
        uADCOut << ts << delim
                << uadcId << delim
                << iasMps << delim
                << aoaDeg << delim
                << aosDeg << delim
                << altM << delim
                << ptPa << delim
                << psPa << '\n';
        newUADCData = false;
    }

    // Autopilot data.
    if (logAP()) {
        apOut << ts << delim
              << rcInTime << delim
              << rcIn1 << delim
              << rcIn2 << delim
              << rcIn3 << delim
              << rcIn4 << delim
              << rcIn5 << delim
              << rcIn6 << delim
              << rcIn7 << delim
              << rcIn8 << delim
              << rcOutTime << delim
              << rcOut1 << delim
              << rcOut2 << delim
              << rcOut3 << delim
              << rcOut4 << delim
              << rcOut5 << delim
              << rcOut6 << delim
              << rcOut7 << delim
              << rcOut8 << '\n';
        newAPData = false;
    }

    if (settings->debugSerial()) {
        qDebug() << "Logger:writeData";
    }
}

// ----------------------------------------------------------------------------
//  Private functions
// ----------------------------------------------------------------------------
void
Logger::openLogFile(QFile &fd, bool &flag, QString type, QString timestamp)
{
    fd.setFileName(QString("%1-%2.csv").arg(type, timestamp));
    flag = fd.open(QFile::WriteOnly | QFile::Truncate);
    if (flag) {
        if (settings->debugSerial()) {
            qDebug() << "Opened log file" << fd.fileName();
        }
    } else {
        qWarning() << "Failed to open log file" << fd.fileName();
        exit(-1);
    }
}


bool
Logger::logAP(void) {
  return apLogFileOpen && haveAP && (!settings->waitForUpdate() || newAPData);
};


bool
Logger::logRIO(void) {
  return rioLogFileOpen && haveRIO &&
    (!settings->waitForUpdate() || newRIOData);
};


bool
Logger::logUADC(void) {
  return uADCLogFileOpen && haveUADC &&
    (!settings->waitForUpdate() || newUADCData);
};


bool
Logger::logVN200(void) {
  return vn200LogFileOpen && haveVN200 &&
    (!settings->waitForUpdate() || newVN200Data);
};

// ----------------------------------------------------------------------------
//  Functions
// ----------------------------------------------------------------------------


};  // namespace dfti
