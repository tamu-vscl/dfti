/*!
 *  \file logger.hh
 *  \brief DFTI logging interface.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#pragma once


// 3rd party
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QTextStream>
#include <QTimer>
// dfti
#include "autopilot/autopilot.hh"
#include "core/consts.hh"
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

    //! Slot to receive data from the Micro Air Data Computer.
    void getUADCData(uADCData data);

    //! Slot to receive data from the VN-200 INS.
    void getVN200Data(VN200Data data);

    //! Slot to write data.
    void writeData(void);

private:
    //! Pointer to settings object.
    Settings *settings{nullptr};

    //! QTimer for writing.
    QTimer *writeTimer{nullptr};

    //! QTimer for flushing log file.
    QTimer *flushTimer{nullptr};

    //! CSV file delimeter.
    const QString delim{","};

    //! Available sensors.
    AvailableSensors sensors{AvailableSensors::NONE};

    //! Flag to indicate log file is opened.
    bool logFileOpen{false};

    //! Flag to indicate if this is our first write.
    bool firstWrite{true};

    //! Flag to indicate an A/P data update.
    bool apDataUpdate{false};

    //! Flag to indicate a uADC data update.
    bool uadcDataUpdate{false};

    //! Flag to indicate a VN-200 data update.
    bool vn200DataUpdate{false};

    //! Flag to indic

    //! Log file.
    QFile logFile;

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

    //! RC Output channel 1 PPM value.
    quint16 rcOut1{0};

    //! RC Output channel 2 PPM value.
    quint16 rcOut2{0};

    //! RC Output channel 3 PPM value.
    quint16 rcOut3{0};

    //! RC Output channel 4 PPM value.
    quint16 rcOut4{0};

    //! RC Output channel 5 PPM value.
    quint16 rcOut5{0};

    //! RC Output channel 6 PPM value.
    quint16 rcOut6{0};

    //! RC Output channel 7 PPM value.
    quint16 rcOut7{0};

    //! RC Output channel 8 PPM value.
    quint16 rcOut8{0};

    //! Indicated Airspeed, meters / second.
    /*!
     *  The indicated airspeed is measured from the uADC 5-hole probe and is
     *  read in with the format XX.XX, which means the highest representable
     *  airspeed is 99.99 m/s, which is about 195 knots.
     */
    float ias_mps{0};

    //! Angle-of-Attack, degrees.
    /*!
     *  The angle-of-attack is represented as +/-XX.XX.
     */
    float aoa_deg{0};

    //! Sideslip Angle, degrees.
    /*!
     *  The sideslip angle is represented as +/-XX.XX.
     */
    float aos_deg{0};

    //! Pressure altitude, meters.
    quint16 alt_m{0};

    //! Total Pressure, Pa.
    quint32 pt_pa{0};

    //! Static Pressure, Pa.
    quint32 ps_pa{0};

    //! GPS time
    /*!
     *  The GPS time is represented as the absolute number of nanoseconds from
     *  the GPS epoch, January 6, 1980 at 0000 UTC.
     */
    quint64 gpsTimeNs{0};

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

    //! Magnetic field, temperature, and pressure.
    /*!
     *  Compensated magnetic field from the IMU in Gauss.
     */
    float mag[3]{0};

    //! Temperature
    /*!
     *  IMU temperature measurement in Celsius.
     */
    float tempC{0};

    //! Pressure
    /*!
     *  IMU pressure measurement in kPA.
     */
    float pressureKpa{0};
};


};  // namespace dfti
