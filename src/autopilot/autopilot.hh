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
#include <QDebug>
#include <QObject>
#include <mavlink/v1/common/mavlink.h>
// dfti
#include "mavlink_info.hh"
#include "sensor/serialsensor.hh"
#include "settings/settings.hh"
#include "util/util.hh"


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
    //! RC input timestamp.
    quint32 rcInTime;
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
    //! RC output timestamp.
    quint32 rcOutTime;
    //! RC output channel 1 PPM value.
    quint16 rcOut1;
    //! RC output channel 2 PPM value.
    quint16 rcOut2;
    //! RC output channel 3 PPM value.
    quint16 rcOut3;
    //! RC output channel 4 PPM value.
    quint16 rcOut4;
    //! RC output channel 5 PPM value.
    quint16 rcOut5;
    //! RC output channel 6 PPM value.
    quint16 rcOut6;
    //! RC Output channel 7 PPM value.
    quint16 rcOut7;
    //! RC Output channel 8 PPM value.
    quint16 rcOut8;
};


//! Serial driver to acquire data from a MAVLink-based autopilot.
class Autopilot : public SerialSensor
{
    Q_OBJECT;

public:
    //! Constructor
    /*!
     *  \param _settings Pointer to Settings object.
     *  \param _parent Pointer to parent QObject.
     */
    explicit Autopilot(Settings *_settings, QObject* _parent = nullptr);

    //! Opens the serial port.
    /*!
     *  Overrides the SerialSensor::open method to open the serial port as R/W.
     */
    void open(void);


    //! Request a MAVLink message at a given rate.
    /*!
     *  \deprecated REQUEST_DATA_STREAM is deprecated in favor of the
     *      MESSAGE_INTERVAL MAVLink message. However, at this time APM does
     *      not support this latter interface.
     *  \remark See http://mavlink.org/messages/common for the MAVLink
     *      MAV_DATA_STREAM enum.
     *  \param streamId The MAVLink stream ID.
     *  \param streamRate Requested rate of the stream in Hz.
     *  \param enabled Use 1 to enable the stream and 0 to disabled.
     */
    void requestStream(quint8 streamId, quint16 streamRate, quint8 enabled);

    //! Request current MAVLink message data rate.
    /*!
     *  \remark See http://mavlink.org/messages/common for MAVLink message
     *  info. Note also that this should return a MESSAGE_INTERVAL message, so
     *  you should make sure this message is handled.
     *  \param msgId The MAVLink message ID.
     */
    void getDataRate(quint16 msgId);

    //! Request a MAVLink message at a given rate.
    /*!
     *  \remark See http://mavlink.org/messages/common for MAVLink message info.
     *  \param msgId The MAVLink message ID.
     *  \param msgRate Requested rate of the message in microseconds. To
     *      disable output, use -1, and to reset to the default rate, use 0.
     */
    void setDataRate(quint8 msgId, float msgRate);

public slots:
    //! Slot to read in data over serial and parse complete packets.
    void readData(void);

signals:
    //! Emitted to share new APData.
    void measurementUpdate(APData data);

private:
    //! Have we gotten a message?
    bool gotMsg{false};

    //! System ID.
    quint8 systemId{0};

    //! Autopilot ID.
    quint8 compId{0};

    //! This ID
    quint8 thisId{255};

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
