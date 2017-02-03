/*!
 *  \file logger.cc
 *  \brief DFTI logging interface implementation.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
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
    QString timestamp = UTC.toString(Qt::ISODate);
    // Open log files.
    openLogFile(apLogFile, apLogFileOpen, "autopilot", timestamp);
    openLogFile(uADCLogFile, uADCLogFileOpen, "uadc", timestamp);
    openLogFile(vn200LogFile, vn200LogFileOpen, "vn200", timestamp);
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
}


void
Logger::enableUADC(uADC *adc)
{
    haveUADC = true;
    connect(adc, &uADC::measurementUpdate, this, &Logger::getUADCData);
}


void
Logger::enableVN200(VN200 *ins)
{
    haveVN200 = true;
    connect(ins, &VN200::measurementUpdate, this, &Logger::getVN200Data);
    connect(ins, &VN200::gpsAvailable, this, &Logger::gpsAvailable);
}


void
Logger::start(void)
{
    writeTimer = new QTimer(this);
    connect(writeTimer, &QTimer::timeout, this, &Logger::writeData);
    flushTimer = new QTimer(this);
    connect(flushTimer, &QTimer::timeout, this, &Logger::flush);
    writeTimer->start(10);  // 10 ms = 100 Hz
    flushTimer->start(1e4);  // 1e4 ms = 10 s
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
    rcIn1 = data.rcIn1;
    rcIn2 = data.rcIn2;
    rcIn3 = data.rcIn3;
    rcIn4 = data.rcIn4;
    rcIn5 = data.rcIn5;
    rcIn6 = data.rcIn6;
    rcIn7 = data.rcIn7;
    rcIn8 = data.rcIn8;
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
        quaternion[i] = data.quaternion[i];
        angularRatesRPS[i] = data.angularRatesRPS[i];
        posDegDegM[i] = data.posDegDegM[i];
        velNedMps[i] = data.velNedMps[i];
        accelMps2[i] = data.accelMps2[i];
        mag[i] = data.mag[i];
    }
    quaternion[3] = data.quaternion[3];
    tempC = data.tempC;
    pressureKpa = data.pressureKpa;
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
    QTextStream uADCOut(&uADCLogFile);
    QTextStream vn200Out(&vn200LogFile);
    apOut.setRealNumberNotation(QTextStream::FixedNotation);
    uADCOut.setRealNumberNotation(QTextStream::FixedNotation);
    vn200Out.setRealNumberNotation(QTextStream::FixedNotation);

    if (firstWrite) {
        // VN-200 data.
        if (vn200LogFileOpen && haveVN200) {
            vn200Out << "unix_time" << delim
                     << "gps_time_ns" << delim
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
                     << "Az_mps2" << delim
                     << "Mx_gauss" << delim
                     << "My_gauss" << delim
                     << "Mz_gauss" << delim
                     << "temp_c" << delim
                     << "pressure_kpa" << '\n';
        }
        // Air data system data.
        if (uADCLogFileOpen && haveUADC) {
            uADCOut << "unix_time" << delim
                    << "uadc_id" << delim
                    << "ias_mps" << delim
                    << "aoa_deg" << delim
                    << "aos_deg" << delim
                    << "alt_m" << delim
                    << "pt_pa" << delim
                    << "ps_pa" << '\n';
        }
        // Autopilot data.
        if (apLogFileOpen && haveAP) {
            apOut << "unix_time" << delim
                  << "rc_in_1_pwm" << delim
                  << "rc_in_2_pwm" << delim
                  << "rc_in_3_pwm" << delim
                  << "rc_in_4_pwm" << delim
                  << "rc_in_5_pwm" << delim
                  << "rc_in_6_pwm" << delim
                  << "rc_in_7_pwm" << delim
                  << "rc_in_8_pwm" << delim
                  << "rc_out_1_pwm" << delim
                  << "rc_out_2_pwm" << delim
                  << "rc_out_3_pwm" << delim
                  << "rc_out_4_pwm" << delim
                  << "rc_out_5_pwm" << delim
                  << "rc_out_6_pwm" << delim
                  << "rc_out_7_pwm" << delim
                  << "rc_out_8_pwm" << '\n';
        }
        firstWrite = false;
    }

    // System time in microseconds.
    quint64 ts = getTimeUsec();

    // VN-200 data.
    if (vn200LogFileOpen && haveVN200 &&
          (!settings->waitForUpdate() || newVN200Data)) {
        vn200Out.setRealNumberPrecision(7);  // float
        vn200Out << ts << delim
                 << gpsTimeNs << delim
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
                 << accelMps2[2] << delim
                 << mag[0] << delim
                 << mag[1] << delim
                 << mag[2] << delim
                 << tempC << delim
                 << pressureKpa << '\n';
        newVN200Data = false;
    }

    // Air data system data.
    if (uADCLogFileOpen && haveUADC &&
          (!settings->waitForUpdate() || newUADCData)) {
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
    if (apLogFileOpen && haveAP && (!settings->waitForUpdate() || newAPData)) {
        apOut << ts << delim
              << rcIn1 << delim
              << rcIn2 << delim
              << rcIn3 << delim
              << rcIn4 << delim
              << rcIn5 << delim
              << rcIn6 << delim
              << rcIn7 << delim
              << rcIn8 << delim
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
//  Public functions
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

// ----------------------------------------------------------------------------
//  Functions
// ----------------------------------------------------------------------------


};  // namespace dfti
