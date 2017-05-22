/*!
 *  \file uadc.hh
 *  \brief Aeroprobe Micro Air Data Computer interface.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016-2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license BSD 2-Clause License
 */
#pragma once


// 3rd party
#include <QByteArray>
#include <QDebug>
#include <QObject>
// dfti
#include "sensor/serialsensor.hh"
#include "settings/settings.hh"


namespace dfti {


//! uADC packet terminator.
const char uadcTerm = '\n';
//! uADC packet length.
const quint8 uadcPktLen = 56;
//! uADC packet checksum pos.
const quint8 uadcPktCksumPos = 53;
//! uADC packet velocity byte start position.
const quint8 uadcPktIasPos = 7;
//! uADC packet velocity byte length.
const quint8 uadcPktIasLen = 5;
//! uADC packet AoA byte start position.
const quint8 uadcPktAoAPos = 14;
//! uADC packet AoA byte length.
const quint8 uadcPktAoALen = 6;
//! uADC packet AoS byte start position.
const quint8 uadcPktAoSPos = 22;
//! uADC packet AoS byte length.
const quint8 uadcPktAoSLen = 6;
//! uADC packet altitude byte start position.
const quint8 uadcPktAltPos = 30;
//! uADC packet altitude byte length.
const quint8 uadcPktAltLen = 5;
//! uADC packet total pressure byte start position.
const quint8 uadcPktPtPos = 37;
//! uADC packet total pressure byte length.
const quint8 uadcPktPtLen = 6;
//! uADC packet static pressure byte start position.
const quint8 uadcPktPsPos = 45;
//! uADC packet static pressure byte length.
const quint8 uadcPktPsLen = 6;


//! Validate the uADC packet checksum.
/*!
 *  The checksum is a simple byte-wise XOR up to but not including the
 *  checksum byte itself.
 *
 *  \param pkt A full uADC packet to validate.
 *  \return True if the packet checksum is correct.
*/
bool validateUADCChecksum(QByteArray pkt);


//! Structure to hold uADC data.
struct uADCData
{
    //! Sequence number, overflows after 99999
    quint32 id = 0;
    //! Indicated Airspeed, meters / second.
    /*!
     *  The indicated airspeed is measured from the uADC 5-hole probe and is
     *  read in with the format XX.XX, which means the highest representable
     *  airspeed is 99.99 m/s, which is about 195 knots.
     */
    float iasMps = 0;
    //! Angle-of-Attack, degrees.
    /*!
     *  The angle-of-attack is represented as +/-XX.XX.
     */
    float aoaDeg = 0;
    //! Sideslip Angle, degrees.
    /*!
     *  The sideslip angle is represented as +/-XX.XX.
     */
    float aosDeg = 0;
    //! Pressure altitude, meters.
    quint16 altM = 0;
    //! Total Pressure, Pa.
    quint32 ptPa = 0;
    //! Static Pressure, Pa.
    quint32 psPa = 0;
};


//! Serial driver to acquire data from a Micro Air Data Computer.
/*!
 *  Reads in data from an Aeroprobe Micro Air Data Computer over RS-232 serial
 *  and parses the data.
 *
 *  The data packet format is
 *      <tt>XXXXX, XX.XX, +XX.XX, +XX.XX, +XXXX, XXXXXX, XXXXXX, XX\\r\\n</tt>
 *  with the fields having the order:
 *      - sample number
 *      - velocity, m/s
 *      - angle-of-attack, deg
 *      - sideslip angle, deg
 *      - altitude, m
 *      - total pressure, Pa
 *      - static pressure, Pa
 *      - checksum byte
 */
class uADC : public SerialSensor
{
    Q_OBJECT;

public:
    //! Constructor
    /*!
     *  \param _settings Pointer to settings object.
     *  \param _parent Pointer to parent QObject.
     */
    explicit uADC(Settings *_settings, QObject* _parent = nullptr);

public slots:
    //! Slot to read in data over serial and parse complete packets.
    void readData(void);

signals:
    //! Emitted to share new uADCData.
    void measurementUpdate(uADCData data);

private:
    //! Buffer
    /*!
     *  Buffer to hold the raw bytes we read in from the serial port. Since we
     *  do not necessarily read in complete packets at a time, we need to let
     *  the buffer fill up until we have a complete packet and then parse it,
     *  which is the purpose of this buffer.
     */
    QByteArray _buf;

    //! Data structure.
    uADCData data;
};


};  // namespace dfti
