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
    QDateTime local(QDateTime::currentDateTime());
    QDateTime UTC(local.toTimeSpec(Qt::UTC));
    logFile.setFileName(QString("%1.csv").arg(UTC.toString(Qt::ISODate)));
    logFileOpen = logFile.open(QFile::WriteOnly | QFile::Truncate);
    if (logFileOpen) {
        if (settings->debugSerial()) {
            qDebug() << "Opened log file" << logFile.fileName();
        }
    } else {
        qWarning() << "Failed to open log file" << logFile.fileName();
        exit(-1);
    }
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
    sensors |= AvailableSensors::HAVE_AP;
    connect(ap, &Autopilot::measurementUpdate, this, &Logger::getAPData);
}


void
Logger::enableUADC(uADC *adc)
{
    sensors |= AvailableSensors::HAVE_UADC;
    connect(adc, &uADC::measurementUpdate, this, &Logger::getUADCData);
}


void
Logger::enableVN200(VN200 *ins)
{
    sensors |= AvailableSensors::HAVE_VN200;
    connect(ins, &VN200::measurementUpdate, this, &Logger::getVN200Data);
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
    if (logFileOpen) {
        logFile.flush();
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
    apDataUpdate = true;
    if (settings->debugSerial()) {
        qDebug() << "Logger::getAPData";
    }
}


void
Logger::getUADCData(uADCData data)
{
    ias_mps = data.ias_mps;
    aoa_deg = data.aoa_deg;
    aos_deg = data.aos_deg;
    alt_m = data.alt_m;
    pt_pa = data.pt_pa;
    ps_pa = data.ps_pa;
    uadcDataUpdate = true;
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
    vn200DataUpdate = true;
    if (settings->debugSerial()) {
        qDebug() << "Logger::getVN200Data";
    }
}


void
Logger::writeData(void)
{
    if (logFileOpen) {
        QTextStream out(&logFile);
        out.setRealNumberNotation(QTextStream::FixedNotation);
        if (firstWrite) {
            out << "unix_time";
            // VN-200 data.
            if (check(sensors & AvailableSensors::HAVE_VN200)) {
                out << delim
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
                    << "pressure_kpa";
            }
            // Air data system data.
            if (check(sensors & AvailableSensors::HAVE_UADC)) {
                out << delim
                    << "ias_mps" << delim
                    << "aoa_deg" << delim
                    << "aos_deg" << delim
                    << "alt_m" << delim
                    << "pt_pa" << delim
                    << "ps_pa";
            }
            // Autopilot data.
            if (check(sensors & AvailableSensors::HAVE_AP)) {
                out << delim
                    << "rc_out_1_pwm" << delim
                    << "rc_out_2_pwm" << delim
                    << "rc_out_3_pwm" << delim
                    << "rc_out_4_pwm" << delim
                    << "rc_out_5_pwm" << delim
                    << "rc_out_6_pwm" << delim
                    << "rc_out_7_pwm" << delim
                    << "rc_out_8_pwm" << delim
                    << "rc_out_1_pwm" << delim
                    << "rc_out_2_pwm" << delim
                    << "rc_out_3_pwm" << delim
                    << "rc_out_4_pwm" << delim
                    << "rc_out_5_pwm" << delim
                    << "rc_out_6_pwm" << delim
                    << "rc_out_7_pwm" << delim
                    << "rc_out_8_pwm";
            }
            out << '\n';
            firstWrite = false;
        }
        out << getTimeUsec();
        // VN-200 data.
        if (check(sensors & AvailableSensors::HAVE_VN200)) {
            out.setRealNumberPrecision(7);
            out << delim
                << gpsTimeNs << delim
                << quaternion[0] << delim
                << quaternion[1] << delim
                << quaternion[2] << delim
                << quaternion[3] << delim
                << angularRatesRPS[0] << delim
                << angularRatesRPS[1] << delim
                << angularRatesRPS[2] << delim
                << posDegDegM[0] << delim
                << posDegDegM[1] << delim
                << posDegDegM[2] << delim
                << velNedMps[0] << delim
                << velNedMps[1] << delim
                << velNedMps[2] << delim
                << accelMps2[0] << delim
                << accelMps2[1] << delim
                << accelMps2[2] << delim
                << mag[0] << delim
                << mag[1] << delim
                << mag[2] << delim
                << tempC << delim
                << pressureKpa;
        }
        // Air data system data.
        if (check(sensors & AvailableSensors::HAVE_UADC)) {
            // We get two decimal places from the uADC...
            out.setRealNumberPrecision(2);
            out << delim
                << ias_mps << delim
                << aoa_deg << delim
                << aos_deg << delim
                << alt_m << delim
                << pt_pa << delim
                << ps_pa;
        }
        // Autopilot data.
        if (check(sensors & AvailableSensors::HAVE_AP)) {
            out << delim
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
                << rcOut8;
        }
        out << '\n';
    }
}


// ----------------------------------------------------------------------------
//  Functions
// ----------------------------------------------------------------------------


};  // namespace dfti