/*!
 *  \file uadc.hh
 *  \brief Aeroprobe Micro Air Data Computer interface.
 *  \author Joshua Harris <joshua.a.harris@tamu.edu>
 *  \copyright ISC License
 */
#pragma once


// 3rd party
#include <QByteArray>
#include <QObject>
#include <QSerialPort>


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
class uAirDataComputer : public QObject
{
    Q_OBJECT;

public:
    //! Constructor
    /*!
     *  \param d Boolean to turn on qDebug output.
     *  \param _parent Pointer to parent QObject.
     */
    explicit uAirDataComputer(bool d = false, QObject* _parent = nullptr);

    //! Destructor
    ~uAirDataComputer();

    //! Returns true if the serial port is open.
    /*!
     *  \return True if the serial port is open.
     */
    bool isOpen(void);

    //! Opens the serial port.
    /*!
     *  Since we may not initially know which serial port corresponds to which
     *  sensor, we need to wait to open the serial port associated with the
     *  uADC until we know which port it is. Once we do, we call this open
     *  method to start reading the serial port.
     */
    void open(void);

    //! Set the serial port name.
    /*!
     *  Sets the serial port name once we have figured out which serial port
     *  corresponds to the uADC sensor.
     *
     *  \param port The serial port name.
     */
    void setSerialPort(QString port);


public slots:
    //! Slot to read in data over serial and parse complete packets.
    void readData(void);


private:
    //! Validate the uADC packet checksum.
    /*!
     *  The checksum is a simple byte-wise XOR up to but not including the
     *  checksum byte itself.
     *
     *  \param pkt A full uADC packet to validate.
     *  \return True if the packet checksum is correct.
    */
    bool validateChecksum(QByteArray pkt);

    //! Do we display verbose debug output?
    bool _debug = false;

    //! Indicates if serial port passed validation.
    bool _valid_serial = false;

    //! Serial port object.
    QSerialPort* _port = nullptr;

    //! Buffer
    /*!
     *  Buffer to hold the raw bytes we read in from the serial port. Since we
     *  do not necessarily read in complete packets at a time, we need to let
     *  the buffer fill up until we have a complete packet and then parse it,
     *  which is the purpose of this buffer.
     */
    QByteArray _buf;

    //! Indicated Airspeed, meters / second.
    /*!
     *  The indicated airspeed is measured from the uADC 5-hole probe and is
     *  read in with the format XX.XX, which means the highest representable
     *  airspeed is 99.99 m/s, which is about 195 knots.
     */
    float _ias_mps = 0.0;

    //! Angle-of-Attack, degrees.
    /*!
     *  The angle-of-attack is represented as +/-XX.XX.
     */
   float _aoa_deg = 0.0;

    //! Sideslip Angle, degrees.
    /*!
     *  The sideslip angle is represented as +/-XX.XX.
     */
   float _aos_deg = 0.0;

    //! Pressure altitude, meters.
    quint16 _alt_m = 0;

    //! Total Pressure, Pa.
    quint32 _pt_pa = 0;

    //! Static Pressure, Pa.
    quint32 _ps_pa = 0;
};


};  // namespace dfti
