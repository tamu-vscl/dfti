/*!
 *  \file rio.hh
 *  \brief Remote I/O logging interface.
 *  \author Joshua Harris
 *  \copyright Copyright © 2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#pragma once


// stdlib
#include <vector>
// 3rd party
#include <QByteArray>
#include <QDebug>
#include <QObject>
// dfti
#include "sensor/serialsensor.hh"
#include "settings/settings.hh"
#include "util/util.hh"


#define ONE_BYTE 2  // Byte length for hex characters.


namespace dfti {


//! RIO packet separator.
const char rioSep = '$';
//! RIO packet terminator.
const char rioTerm = '\n';
//! RIO packet start.
const QString rioStart{"$$$"};
//! RIO packet terminator string.
const QString rioTermStr{"\r\n"};


//! Validate the RIO packet checksum.
/*!
 *  The checksum is a simple byte-wise XOR up to but not including the
 *  checksum byte itself.
 *
 *  \param pkt A full packet to validate.
 *  \return True if the packet checksum is correct.
*/
bool validateRIOChecksum(QByteArray pkt);


//! Structure to hold control effector data.
struct RIOData
{
    //! Vector of RIO values.
    std::vector<float> values;
};


//! Serial driver to acquire data from a generic Remote I/O device.
/*!
 *  Reads in data from a generic RIO over a serial port and parses the data.
 *
 *  The data packet format is
 *      <tt>$$$field_1$field_2$...$field_n$checksum\\r\\n</tt>
 *  with the checksum byte being represented in hex.
 */
class RIO : public SerialSensor
{
    Q_OBJECT;

public:
    //! Constructor
    /*!
     *  \param _settings Pointer to settings object.
     *  \param _parent Pointer to parent QObject.
     */
    explicit RIO(Settings *_settings, QObject* _parent = nullptr);

public slots:
    //! Slot to read in data over serial and parse complete packets.
    void readData(void);

signals:
    //! Emitted to share new RIOData.
    void measurementUpdate(RIOData data);

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
    RIOData data;
};


};  // namespace dfti
