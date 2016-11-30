/*!
 *  \file vn200.hh
 *  \brief VectorNav VN-200 Inertial Navigation System interface.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#pragma once


// stdlib
#include <cmath>
#include <cstring>
// 3rd party
#include <QDebug>
#include <QObject>
// dfti
#include "sensor/serialsensor.hh"
#include "settings/settings.hh"
#include "util/util.hh"


namespace dfti {


//! Validate the VN-200 packet checksum.
/*!
 *  The checksum is a CRC 16; if the checksum byte is included the checksum
 *  should evaluate to 0.
 *
 *  \param pkt A full VN-200 packet to validate.
 *  \param pktLen The length of the packet.
 *  \return True if the packet checksum is correct.
*/
bool validateVN200Checksum(quint8 *pkt, quint8 pktLen);


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


//! Serial driver to acquire data from a VN-200 Inertial Navigation System.
/*!
 *  Reads in data from a VectorNav VN-200 Inertial Navigation System over
 *  serial and parses the data.
 *
 *  The data packet format is binary, with a header consisting of
 *
 *  - a sync byte (0xfa)
 *  - the selected output groups (bitmask, 1 byte)
 *  - 16-bit bitmasks for the selected outputs from the groups
 *
 *  In the configuration this code assumes, Output Group 1 is selected, and the
 *
 *  - TimeGps
 *  - Quaternion
 *  - AngularRate
 *  - Position
 *  - Velocity
 *  - Accel
 *  - MagPres
 *
 *  fields are selected.
 *  The last two bytes are the checksum.
 */
class VN200 : public SerialSensor
{
    Q_OBJECT;

public:
    //! Constructor
    /*!
     *  \param _settings Pointer to settings object.
     *  \param _parent Pointer to parent QObject.
     */
    explicit VN200(Settings *_settings, QObject* _parent = nullptr) :
        SerialSensor(_settings, _parent) { };

public slots:
    //! Slot to read in data over serial and parse complete packets.
    void readData(void);

signals:
    //! Emitted when GPS data is available.
    void gpsAvailable(bool flag);

    //! Emitted to share new VN200Data.
    void measurementUpdate(VN200Data data);

private:
    //! Copy from raw packet to data struct.
    void copyPacketToData(void);

    //! Flag to indicate if we are reading a new packet.
    /*!
     *  Since we read in the packet a single byte at a time, the way we handle
     *  a byte depends on if we are looking for a new packet. If that's the
     *  case, we need to check for the sync byte and then the group byte.
     */
    bool foundSyncByte{false};
    bool foundGroupByte{false};

    //! Sync byte.
    static const quint8 syncByte{0xfa};

    //! Group byte
    static const quint8 groupByte{0x01};

    //! Buffer position.
    quint8 currentBufIdx{0};

    //! Expected packet size.
    static const quint8 packetSize{110};

    //! Buffer
    /*!
     *  Buffer to hold the raw bytes we read in from the serial port. We read
     *  in one byte at a time.
     */
    quint8 buf[packetSize];

#pragma pack(push, 1)  // change structure packing to 1 byte
    //! Packet format.
    /*!
     *  Note that we need to use pragma pack(1) for the memory layout of the
     *  structure to match the wire format of the packet, but we want it ONLY
     *  for this struct (which, incidentally, is POD).
     */
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
#pragma pack(pop)  // reset structure packing

    //! Output data structure.
    VN200Data data;

    //! Raw packet data.
    VN200Packet *packet{nullptr};
};


};  // namespace dfti
