/*!
 *  \file autopilot.cc
 *  \brief MAVLink-based autopilot interface implementation.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#include "autopilot.hh"


namespace dfti {


// ----------------------------------------------------------------------------
//  Constructors/destructors
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  Public functions
// ----------------------------------------------------------------------------
void
Autopilot::open(void)
{
    if (_valid_serial && !isOpen()) {
        if (_port->open(QIODevice::ReadWrite)) {
            if (settings->debugSerial()) {
                qDebug() << "Opened serial port:"
                         << _port->portName();
            }
        } else {
            if (settings->debugSerial()) {
                qDebug() << "Failed to open serial port:"
                         << _port->errorString();
            }
        };
        connect(_port, &QIODevice::readyRead, this,
            &Autopilot::readData);
    }
}

void
Autopilot::getDataRate(quint16 msgId)
{
    // Create MAVLink command.
    mavlink_command_long_t cmd = {0};
    cmd.target_system = systemId;
    cmd.target_component = compId;
    cmd.command = MAV_CMD_GET_MESSAGE_INTERVAL;
    cmd.confirmation = true;
    cmd.param1 = static_cast<float>(msgId);

    // Create packed MAVLink message.
    char buf[300];
    mavlink_message_t msg;
    mavlink_msg_command_long_encode(systemId, thisId, &msg, &cmd);
    quint32 len = mavlink_msg_to_send_buffer((quint8 *) buf, &msg);

    // Send the command message.
    if (isOpen()) {
        quint32 writeLen = _port->write(buf, len);
        QString msgName = QString::number(msgId);
        if (mavlinkMessageName.contains(msgId)) {
            msgName = mavlinkMessageName[msgId];
        }
        if (settings->debugSerial()) {
            qDebug() << "Requested" << msgName <<  "stream rate.";
        }
        if (writeLen != len) {
            qWarning() << "Failed to send message" << msgName
                       << "to autopilot!";
        }
    }
}


void
Autopilot::setDataRate(quint8 msgId, float msgRate)
{
    // Create MAVLink command.
    mavlink_command_long_t cmd = {0};
    cmd.target_system = systemId;
    cmd.target_component = compId;
    cmd.command = MAV_CMD_SET_MESSAGE_INTERVAL;
    cmd.confirmation = 0;
    cmd.param1 = static_cast<float>(msgId);
    cmd.param2 = msgRate;

    // Create packed MAVLink message.
    char buf[300];
    mavlink_message_t msg;
    mavlink_msg_command_long_encode(systemId, thisId, &msg, &cmd);
    quint32 len = mavlink_msg_to_send_buffer((quint8 *) buf, &msg);

    // Send the command message.
    if (isOpen()) {
        quint32 writeLen = _port->write(buf, len);
        _port->flush();
        QString msgName = QString::number(msgId);
        if (mavlinkMessageName.contains(msgId)) {
            msgName = mavlinkMessageName[msgId];
        }
        if (settings->debugSerial()) {
            qDebug() << "Requested" << msgName <<  "every" << msgRate << "ms";
        }
        if (writeLen != len) {
            qWarning() << "Failed to send message" << msgName
                       << "to autopilot!";
        }
    }
}


void
Autopilot::requestStream(quint8 streamId, quint16 streamRate, quint8 enabled)
{
    // Create MAVLink REQUEST_DATA_STREAM message.
    mavlink_request_data_stream_t stream = {0};
    stream.target_system = systemId;
    stream.target_component = compId;
    stream.req_stream_id = streamId;
    stream.req_message_rate = streamRate;
    stream.start_stop = enabled;

    // Create packed MAVLink message.
    char buf[300];
    mavlink_message_t msg;
    mavlink_msg_request_data_stream_encode(systemId, thisId, &msg, &stream);
    quint32 len = mavlink_msg_to_send_buffer((quint8 *) buf, &msg);

    // Send the message.
    if (isOpen()) {
        quint32 writeLen = _port->write(buf, len);
        _port->flush();
        if (writeLen != len) {
            qWarning() << "Failed to send data stream request to autopilot!";
        }
    }
}

// ----------------------------------------------------------------------------
// Public Slots
// ----------------------------------------------------------------------------
void
Autopilot::readData(void)
{
    quint8 ch;
    mavlink_status_t status;
    quint8 msgReceived = false;

    // Read in a single byte.
    if (_port->getChar((char *) &ch)) {
        // Attempt to parse.
        msgReceived = mavlink_parse_char(MAVLINK_COMM_1, ch, &message, &status);
        // Check if we dropped any packets.
        if (lastStatus.packet_rx_drop_count != status.packet_rx_drop_count) {
            if (settings->debugSerial()) {
                qDebug() << "dropped" << status.packet_rx_drop_count
                         << "packets";
            }
            lastStatus = status;
        }

        // Extract the message type if we have a full packet.
        if (msgReceived) {
            // Get the system and component IDs of the connected a/p.
            systemId = message.sysid;
            compId = message.compid;

            switch (message.msgid) {
                case MAVLINK_MSG_ID_HEARTBEAT:
                    if (settings->debugData()) {
                        qDebug() << "got HEARTBEAT";
                    }
                case MAVLINK_MSG_ID_RC_CHANNELS_RAW: {
                    mavlink_rc_channels_raw_t rcIn;
                    mavlink_msg_rc_channels_raw_decode(&message, &rcIn);
                    data.rcIn1 = rcIn.chan1_raw;
                    data.rcIn2 = rcIn.chan2_raw;
                    data.rcIn3 = rcIn.chan3_raw;
                    data.rcIn4 = rcIn.chan4_raw;
                    data.rcIn5 = rcIn.chan5_raw;
                    data.rcIn6 = rcIn.chan6_raw;
                    data.rcIn7 = rcIn.chan7_raw;
                    data.rcIn8 = rcIn.chan8_raw;
                    timestamps.rcChannelsRaw = getTimeUsec();
                    if (settings->debugData()) {
                        qDebug() << "Autopilot::readData: RC_CHANNELS_RAW";
                    }
                    break;
                }
                case MAVLINK_MSG_ID_SERVO_OUTPUT_RAW: {
                    mavlink_servo_output_raw_t rcOut;
                    mavlink_msg_servo_output_raw_decode(&message, &rcOut);
                    data.rcOut1 = rcOut.servo1_raw;
                    data.rcOut2 = rcOut.servo2_raw;
                    data.rcOut3 = rcOut.servo3_raw;
                    data.rcOut4 = rcOut.servo4_raw;
                    data.rcOut5 = rcOut.servo5_raw;
                    data.rcOut6 = rcOut.servo6_raw;
                    data.rcOut7 = rcOut.servo7_raw;
                    data.rcOut8 = rcOut.servo8_raw;
                    timestamps.servoOutputRaw = getTimeUsec();
                    if (settings->debugData()) {
                        qDebug() << "Autopilot::readData: SERVO_OUTPUT_RAW";
                    }
                    break;
                }
                case MAVLINK_MSG_ID_STATUSTEXT: {
                    mavlink_statustext_t status;
                    mavlink_msg_statustext_decode(&message, &status);
                    qWarning() << "[WARN:" << status.severity << "]: "
                               << status.text;
                }
                case MAVLINK_MSG_ID_COMMAND_ACK: {
                    if (settings->debugData()) {
                        mavlink_command_ack_t ack;
                        mavlink_msg_command_ack_decode(&message, &ack);
                        qDebug() << "COMMAND ACK" << ack.command << "RESULT"
                                 << ack.result;
                    }
                }
                case MAVLINK_MSG_ID_MESSAGE_INTERVAL: {
                    if (settings->debugData()) {
                        mavlink_message_interval_t mi;
                        mavlink_msg_message_interval_decode(&message, &mi);
                        QString msgName = QString::number(mi.message_id);
                        if (mavlinkMessageName.contains(mi.message_id)) {
                            msgName = mavlinkMessageName[mi.message_id];
                        }
                        qDebug() << "Message" << msgName << "at"
                                 << mi.interval_us << "us.";
                    }
                }
                default: {
                    if (settings->debugData()) {
                        QString msgName = QString::number(message.msgid);
                        if (mavlinkMessageName.contains(message.msgid)) {
                            msgName = mavlinkMessageName[message.msgid];
                        }
                        if (settings->use_message_interval()) {
                            setDataRate(message.msgid, -1);
                            getDataRate(message.msgid);
                        }
                        qDebug() << "Got unhandled message type:"
                                 << msgName;
                    }
                    break;
                }
            }
        }
    } else {
        if (settings->debugSerial()) {
            qDebug() << "Failed to read serial port!";
        }
    }

    // Emit message update if we have both, then reset.
    if (timestamps.rcChannelsRaw && timestamps.servoOutputRaw) {
        emit measurementUpdate(data);
        timestamps.reset();

        if (settings->debugData()) {
            qDebug() << "MAVLink:\n"
                     << "\tRCIN1 :  " << data.rcIn1 << "\n"
                     << "\tRCIN2 :  " << data.rcIn2 << "\n"
                     << "\tRCIN3 :  " << data.rcIn3 << "\n"
                     << "\tRCIN4 :  " << data.rcIn4 << "\n"
                     << "\tRCIN5 :  " << data.rcIn5 << "\n"
                     << "\tRCIN6 :  " << data.rcIn6 << "\n"
                     << "\tRCIN7 :  " << data.rcIn7 << "\n"
                     << "\tRCIN8 :  " << data.rcIn8 << "\n"
                     << "\tRCOUT1:  " << data.rcOut1 << "\n"
                     << "\tRCOUT2:  " << data.rcOut2 << "\n"
                     << "\tRCOUT3:  " << data.rcOut3 << "\n"
                     << "\tRCOUT4:  " << data.rcOut4 << "\n"
                     << "\tRCOUT5:  " << data.rcOut5 << "\n"
                     << "\tRCOUT6:  " << data.rcOut6 << "\n"
                     << "\tRCOUT7:  " << data.rcOut7 << "\n"
                     << "\tRCOUT8:  " << data.rcOut8 << "\n";
        }
    }

    // If this is our first time getting data, request the streams/messages we
    // want.
    if (!gotMsg) {
        if (settings->use_message_interval()) {
            setDataRate(MAVLINK_MSG_ID_RC_CHANNELS_RAW,
                hzToUsec(settings->stream_rate()));
            setDataRate(MAVLINK_MSG_ID_SERVO_OUTPUT_RAW,
                hzToUsec(settings->stream_rate()));
            getDataRate(MAVLINK_MSG_ID_RC_CHANNELS_RAW);
            getDataRate(MAVLINK_MSG_ID_SERVO_OUTPUT_RAW);
        } else {
            requestStream(MAV_DATA_STREAM_RC_CHANNELS,
                settings->stream_rate(), 1);
        }
        gotMsg = true;
    }

    return;
}

// ----------------------------------------------------------------------------
//  Private functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  Functions
// ----------------------------------------------------------------------------


};  // namespace dfti
