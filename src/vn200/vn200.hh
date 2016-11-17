/*!
 *  \file vn200.hh
 *  \brief VectorNav VN-200 Inertial Navigation System interface.
 *  \author Joshua Harris
 *  \copyright ISC License
 */
#pragma once
// Need to have speficied alignment in structures to cast.
#pragma pack(1)


// 3rd party
#include <QByteArray>
#include <QObject>
#include <QSerialPort>
// dfti
#include "sensor/serialsensor.hh"


namespace dfti {


//! Validate the uADC packet checksum.
/*!
 *  The checksum is a CRC 16; if the checksum byte is included the checksum
 *  should evaluate to 0.
 *
 *  \param pkt A full VN-200 packet to validate.
 *  \return True if the packet checksum is correct.
*/
bool validateVN200Checksum(QByteArray pkt);


//! Structure to hold VN-200 data.
struct VN200Data
{
    //! GPS time
    /*!
     *  The GPS time is represented as the absolute number of nanoseconds from
     *  the GPS epoch, January 6, 1980 at 0000 UTC.
     */
    quint64 gpsTimeNs = 0;
    //! Quaternion
    /*!
     *  Stores the attitude quaternion giving the body frame with respect to
     *  the local North East Down frame.
     *
     *  Note that the VN-200 uses the last term as the scalar value in its
     *  output, but DFTI uses the first term as the scalar as is common.
     */
    float quaternion[4] = {0};
    //! Angular Rates
    /*!
     *  Stores the body-axis angular rates P, Q, and R in radians per second.
     *  Order is P, Q, R.
     */
    float angularRatesRPS[3] = {0};
    //! Lat-Long-Alt Position
    /*!
     *  Contains the latitude, longitude, and altitude position of the aircraft
     *  in inertial space. The lat/long are in degrees, and the altitude is in
     *  meters.
     *
     *  Order is lat, long, alt.
     */
    float posDegDegM[3] = {0};
    //! NED Velocity
    /*!
     *  Stores the estimated velocity in the North-East-Down frame in m/s.
     *  Order is Vx, Vy, Vz.
     */
    float velNedMps[3] = {0};
    //! Accelerations
    /*!
     *  Body-axis accelerations in m/s^2. Includes the effect of gravity and is
     *  bias compensated by the EKF. Order is Ax, Ay, Az.
     */
    float accelMps2[3] = {0};
    //! Magnetic field, temperature, and pressure.
    /*!
     *  Compensated magnetic field from the IMU in Gauss.
     */
    float mag[3] = {0};
    //! Temperature
    /*!
     *  IMU temperature measurement in Celsius.
     */
    float tempC = 0;
    //! Pressure
    /*!
     *  IMU pressure measurement in kPA.
     */
    float pressureKpa = 0;
};


//! Serial driver to acquire data from a Micro Air Data Computer.
/*!
 *  Reads in data from an Aeroprobe Micro Air Data Computer over RS-232 serial
 *  and parses the data.
 *
 *  The data packet format is binary, with a header consisting of
 *      - a sync byte (0xfa)
 *      - the selected output groups (bitmask, 1 byte)
 *      - 16-bit bitmasks for the selected outputs from the groups
 *  In the configuration this code assumes, Output Group 1 is selected, and the
 *      - TimeGps
 *      - Quaternion
 *      - AngularRate
 *      - Position
 *      - Velocity
 *      - Accel
 *      - MagPres
 *  fields are selected.
 *  The last two bytes are the checksum.
 */
class VN200 : public SerialSensor
{
    Q_OBJECT;

public:
    //! Constructor
    /*!
     *  \param d Boolean to turn on qDebug output.
     *  \param _parent Pointer to parent QObject.
     */
    explicit VN200(bool d = false, QObject* _parent = nullptr):
        SerialSensor(d, _parent) { };

public slots:
    //! Slot to read in data over serial and parse complete packets.
    void readData(void);

signals:
    void measurementUpdate(VN200Data data);

private:
    //! Copy from raw packet to data struct.
    void copyPacketToData(void);

    //! Header
    /*!
     *  The header is the sync byte, group byte, and field bitmask. This
     *  bitmask is 2**1 | 2**4 | 2**5 | 2**6 | 2**7 | 2**8 | 2**10, which
     *  evaluates to 0x05f2, or, in little endian form, {0xf2, 0x05}.
     */
    const QByteArray header{"\xfa\x01\xf2\x05", 4};
    const quint8 packetSize = 108;

    //! Packet format.
    struct VN200Packet {
        quint8 sync = 0;
        quint8 outputGroups = 0;
        quint16 outputFields = 0;
        quint64 timeGPS = 0;
        float quaternion[4] = {0};
        float angularRate[3] = {0};
        double position[3] = {0};
        float velocity[3] = {0};
        float accel[3] = {0};
        float mag[3] = {0};
        float temp = 0;
        float pressure = 0;
    };

    //! Output data structure.
    VN200Data data;

    //! Raw packet data.
    VN200Packet *packet = nullptr;
};


};  // namespace dfti
