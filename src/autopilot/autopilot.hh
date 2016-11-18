/*!
 *  \file autopilot.hh
 *  \brief MAVLink-supporting autopilot interface.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#pragma once


// 3rd party
#include <QByteArray>
#include <QObject>
#include <QSerialPort>
#include <mavlink/common/mavlink.h>
// dfti
#include "sensor/serialsensor.hh"


namespace dfti {


//! Structure to hold autopilot data.
/*!
 *  The autopilot is used to obtain pilot commands and commanded servo
 *  positions via the RC_CHANNELS_RAW and SERVO_OUTPUT_RAW messages. These
 *  messages give the values as raw PPM values. PPM modulation is 1000 us = 0%,
 *  2000 us = 100%, although some tx/rx combos may be different.
 */
struct APData
{
    //! RC input channel 1 PPM value.
    quint16 rcIn1;
    //! RC input channel 2 PPM value.
    quint16 rcIn2;
    //! RC input channel 3 PPM value.
    quint16 rcIn3;
    //! RC input channel 4 PPM value.
    quint16 rcIn4;
    //! RC input channel 5 PPM value.
    quint16 rcIn5;
    //! RC input channel 6 PPM value.
    quint16 rcIn6;
    //! RC input channel 7 PPM value.
    quint16 rcIn7;
    //! RC input channel 8 PPM value.
    quint16 rcIn8;
    //! RC Output channel 1 PPM value.
    quint16 rcOut1;
    //! RC Output channel 2 PPM value.
    quint16 rcOut2;
    //! RC Output channel 3 PPM value.
    quint16 rcOut3;
    //! RC Output channel 4 PPM value.
    quint16 rcOut4;
    //! RC Output channel 5 PPM value.
    quint16 rcOut5;
    //! RC Output channel 6 PPM value.
    quint16 rcOut6;
    //! RC Output channel 7 PPM value.
    quint16 rcOut7;
    //! RC Output channel 8 PPM value.
    quint16 rcOut8;
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
class Autopilot : public SerialSensor
{
    Q_OBJECT;

public:
    //! Constructor
    /*!
     *  \param d Boolean to turn on qDebug output.
     *  \param _parent Pointer to parent QObject.
     */
    explicit Autopilot(bool d = false, QObject* _parent = nullptr) :
        SerialSensor(d, _parent) { lastStatus.packet_rx_drop_count = 0; };

public slots:
    //! Slot to read in data over serial and parse complete packets.
    void readData(void);

signals:
    void measurementUpdate(APData data);

private:
    //! Current MAVLink message.
    mavlink_message_t message;

    //! Last MAVLink message status.
    /*!
     *  The last MAVLink message status is kept to determine if we have packet
     *  loss by comparing the packet_rx_drop_count field.
     */
    mavlink_status_t lastStatus;

    //! Hold timestamps for MAVLink messages we want to make sure we get both.
    struct MavlinkTimestamps {
        //! Ctor.
        MavlinkTimestamps() { reset(); }
        //! Message timestamp for the RC_CHANNELS_RAW message.
        quint64 rcChannelsRaw;
        //! Message timestamp for the SERVO_OUTPUT_RAW message.
        quint64 servoOutputRaw;
        //! Reset the timestamps to zero.
        void reset(void) { rcChannelsRaw = 0; servoOutputRaw = 0; }
    };

    //! Hold the current timestamps for received MAVLink messages.
    MavlinkTimestamps timestamps;

    //! Output data structure.
    APData data;
};


};  // namespace dfti
