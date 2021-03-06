/*!
 *  \file logger.hh
 *  \brief DFTI logging interface.
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


// stdlib
#include <vector>
// 3rd party
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QProcess>
#include <QTextStream>
#include <QTimer>
// dfti
#include "autopilot/autopilot.hh"
#include "core/consts.hh"
#include "core/qptrutil.hh"
#include "rio/rio.hh"
#include "settings/settings.hh"
#include "uadc/uadc.hh"
#include "util/util.hh"
#include "vn200/vn200.hh"


namespace dfti {


//! Receives data and logs to file.
class Logger : public QObject
{
    Q_OBJECT;

public:
    //! Constructor
    /*!
     *  \param _settings Pointer to Settings object.
     *  \param _parent Pointer to parent QObject.
     */
    explicit Logger(Settings *_settings, QObject* _parent = nullptr);

    //! Dtor.
    ~Logger();

    //! Enable Autopilot Sensor.
    /*!
     * \param ap Pointer to Autopilot object.
     */
    void enableAutopilot(Autopilot *ap);

    //! Enable Micro Air Data Computer Sensor.
    /*!
     * \param adc Pointer to uADC object.
     */
    void enableUADC(uADC *adc);

    //! Enable Remote I/O unit.
    /*!
     * \param rio Pointer to RIO object.
     */
    void enableRIO(RIO *rio);

    //! Enable VN-200 INS Sensor.
    /*!
     * \param ins Pointer to VN200 object.
     */
    void enableVN200(VN200 *ins);

    //! Start logging.
    /*!
     *  Connects QTimers to the flush and writeData slots.
     */
    void start(void);

public slots:
    //! Slot to flush the data buffer.
    void flush(void);

    //! Slot to receive data from the autopilot.
    void getAPData(APData data);

    //! Slot to receive data from the RIO.
    void getRIOData(RIOData data);

    //! Slot to receive data from the Micro Air Data Computer.
    void getUADCData(uADCData data);

    //! Slot to receive data from the VN-200 INS.
    void getVN200Data(VN200Data data);

    //! Slot to see if GPS is available.
    void gpsAvailable(bool flag);

    //! Slot to write data.
    void writeData(void);

private:
    //! Function to open a log file.
    /*!
     *  \param fd Reference to QFile.
     *  \param flag Reference to file open flag.
     *  \param time Sensor type.
     *  \param timestamp Formatted current timestamp.
     */
    void openLogFile(QFile &fd, bool &flag, QString type, QString timestamp);

    //! Function to determine if MAVLink data should be logged.
    bool logAP(void);

    //! Function to determine if RIO data should be logged.
    bool logRIO(void);

    //! Function to determine if uADC data should be logged.
    bool logUADC(void);

    //! Function to determine if VN-200 data should be logged.
    bool logVN200(void);

    //! Pointer to settings object.
    QPointer<Settings> settings{nullptr};

    //! QTimer for writing.
    QPointer<QTimer> writeTimer{nullptr};

    //! QTimer for flushing log file.
    QPointer<QTimer> flushTimer{nullptr};

    //! Log file timestamp.
    QString timestamp{""};

    //! CSV file delimeter.
    const QString delim{","};

    //! Autopilot available.
    bool haveAP{false};

    //! RIO available.
    bool haveRIO{false};

    //! uADC available.
    bool haveUADC{false};

    //! VN-200 available.
    bool haveVN200{false};

    //! Flag to indicate autopilot log file is opened.
    bool apLogFileOpen{false};

    //! Flag to indicate RIO log file is opened.
    bool rioLogFileOpen{false};

    //! Flag to indicate uADC log file is opened.
    bool uADCLogFileOpen{false};

    //! Flag to indicate VN-200 log file is opened.
    bool vn200LogFileOpen{false};

    //! Flag to indicate if this is our first write.
    bool firstWrite{true};

    //! Flag to indicate an A/P data update.
    bool newAPData{false};

    //! Flag to indicate a RIO data update.
    bool newRIOData{false};

    //! Flag to indicate a uADC data update.
    bool newUADCData{false};

    //! Flag to indicate a VN-200 data update.
    bool newVN200Data{false};

    //! Flag to indicate GPS is available.
    bool haveGPS{false};

    //! Flag to indicate we have set the system time.
    bool setSystemTime{false};

    //! Autopilot log file.
    QFile apLogFile;

    //! RIO log file
    QFile rioLogFile;

    //! uADC log file
    QFile uADCLogFile;

    //! VN-200 log file.
    QFile vn200LogFile;

    //! RC input timestamp.
    quint32 rcInTime{0};

    //! RC input channel 1 PPM value.
    quint16 rcIn1{0};

    //! RC input channel 2 PPM value.
    quint16 rcIn2{0};

    //! RC input channel 3 PPM value.
    quint16 rcIn3{0};

    //! RC input channel 4 PPM value.
    quint16 rcIn4{0};

    //! RC input channel 5 PPM value.
    quint16 rcIn5{0};

    //! RC input channel 6 PPM value.
    quint16 rcIn6{0};

    //! RC input channel 7 PPM value.
    quint16 rcIn7{0};

    //! RC input channel 8 PPM value.
    quint16 rcIn8{0};

    //! RC output timestamp.
    quint32 rcOutTime{0};

    //! RC output channel 1 PPM value.
    quint16 rcOut1{0};

    //! RC output channel 2 PPM value.
    quint16 rcOut2{0};

    //! RC output channel 3 PPM value.
    quint16 rcOut3{0};

    //! RC output channel 4 PPM value.
    quint16 rcOut4{0};

    //! RC output channel 5 PPM value.
    quint16 rcOut5{0};

    //! RC output channel 6 PPM value.
    quint16 rcOut6{0};

    //! RC output channel 7 PPM value.
    quint16 rcOut7{0};

    //! RC output channel 8 PPM value.
    quint16 rcOut8{0};

    //! RIO data vector.
    std::vector<float> rioData;

    //! uADC packet ID.
    /*!
     *  \remark This is only useful for making sure we don't drop uADC packets.
     */
    quint32 uadcId{0};

    //! Indicated Airspeed, meters / second.
    /*!
     *  The indicated airspeed is measured from the uADC 5-hole probe and is
     *  read in with the format XX.XX, which means the highest representable
     *  airspeed is 99.99 m/s, which is about 195 knots.
     */
    float iasMps{0};

    //! Angle-of-Attack, degrees.
    /*!
     *  The angle-of-attack is represented as +/-XX.XX.
     */
    float aoaDeg{0};

    //! Sideslip Angle, degrees.
    /*!
     *  The sideslip angle is represented as +/-XX.XX.
     */
    float aosDeg{0};

    //! Pressure altitude, meters.
    quint16 altM{0};

    //! Total Pressure, Pa.
    quint32 ptPa{0};

    //! Static Pressure, Pa.
    quint32 psPa{0};

    //! GPS time
    /*!
     *  The GPS time is represented as the absolute number of nanoseconds from
     *  the GPS epoch, January 6, 1980 at 0000 UTC.
     */
    quint64 gpsTimeNs{0};

    //! Euler Angles
    /*!
     *  Stores the 3-2-1 Euler angles psi, theta, phi in degrees. Order is yaw,
     *  pitch, roll.
     */
    float eulerDeg[3] = {0};

    //! Quaternion
    /*!
     *  Stores the attitude quaternion giving the body frame with respect to
     *  the local North East Down frame.
     *
     *  Note that the VN-200 uses the last term as the scalar value in its
     *  output, but DFTI uses the first term as the scalar as is common.
     */
    float quaternion[4]{0};

    //! Angular Rates
    /*!
     *  Stores the body-axis angular rates P, Q, and R in radians per second.
     *  Order is P, Q, R.
     */
    float angularRatesRPS[3]{0};

    //! Lat-Long-Alt Position
    /*!
     *  Contains the latitude, longitude, and altitude position of the aircraft
     *  in inertial space. The lat/long are in degrees, and the altitude is in
     *  meters.
     *
     *  Order is lat, long, alt.
     */
    float posDegDegM[3]{0};

    //! NED Velocity
    /*!
     *  Stores the estimated velocity in the North-East-Down frame in m/s.
     *  Order is Vx, Vy, Vz.
     */
    float velNedMps[3]{0};

    //! Accelerations
    /*!
     *  Body-axis accelerations in m/s^2. Includes the effect of gravity and is
     *  bias compensated by the EKF. Order is Ax, Ay, Az.
     */
    float accelMps2[3]{0};
};


};  // namespace dfti
