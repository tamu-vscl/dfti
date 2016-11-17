/*!
 *  \file uadc.hh
 *  \brief Aeroprobe Micro Air Data Computer interface.
 *  \author Joshua Harris
 *  \copyright ISC License
 */
#pragma once


// 3rd party
#include <QByteArray>
#include <QObject>
#include <QSerialPort>
// dfti
#include "sensor/serialsensor.hh"


namespace dfti {


// uADC packet terminator.
const char uadcTerm = '\n';
// uADC packet length.
const quint8 uadcPktLen = 56;
// uADC packet checksum pos.
const quint8 uadcPktCksumPos = 53;
// uADC packet velocity info.
const quint8 uadcPktIasPos = 7;
const quint8 uadcPktIasLen = 5;
// uADC packet AoA info.
const quint8 uadcPktAoAPos = 14;
const quint8 uadcPktAoALen = 6;
// uADC packet AoS info.
const quint8 uadcPktAoSPos = 22;
const quint8 uadcPktAoSLen = 6;
// uADC packet altitude info.
const quint8 uadcPktAltPos = 30;
const quint8 uadcPktAltLen = 5;
// uADC packet total pressure info.
const quint8 uadcPktPtPos = 37;
const quint8 uadcPktPtLen = 6;
// uADC packet static pressure info.
const quint8 uadcPktPsPos = 45;
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
    quint32 id;
    //! Indicated Airspeed, meters / second.
    /*!
     *  The indicated airspeed is measured from the uADC 5-hole probe and is
     *  read in with the format XX.XX, which means the highest representable
     *  airspeed is 99.99 m/s, which is about 195 knots.
     */
    float ias_mps;
    //! Angle-of-Attack, degrees.
    /*!
     *  The angle-of-attack is represented as +/-XX.XX.
     */
    float aoa_deg;
    //! Sideslip Angle, degrees.
    /*!
     *  The sideslip angle is represented as +/-XX.XX.
     */
    float aos_deg;
    //! Pressure altitude, meters.
    quint16 alt_m;
    //! Total Pressure, Pa.
    quint32 pt_pa;
    //! Static Pressure, Pa.
    quint32 ps_pa;
};
typedef struct uADCData uADCData_s;


//! Serial driver to acquire data from a Micro Air Data Computer.
/*!
 *  Reads in data from an Aeroprobe Micro Air Data Computer over RS-232 serial
 *  and parses the data.
 *
 *  The data packet format is
 *      XXXXX, XX.XX, +XX.XX, +XX.XX, +XXXX, XXXXXX, XXXXXX, XX\r\n
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
     *  \param d Boolean to turn on qDebug output.
     *  \param _parent Pointer to parent QObject.
     */
    explicit uADC(bool d = false, QObject* _parent = nullptr) :
        SerialSensor(d, _parent) { };

public slots:
    //! Slot to read in data over serial and parse complete packets.
    void readData(void);

signals:
    void measurementUpdate(uADCData_s data);

private:
    //! Data structure.
    uADCData_s data;
};


};  // namespace dfti
