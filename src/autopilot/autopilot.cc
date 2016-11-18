/*!
 *  \file autopilot.cc
 *  \brief MAVLink-based autopilot interface implementation.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */


// stdlib
#include <cstring>
// 3rd party
#include <QDebug>
#include <QIODevice>
#include <QSerialPortInfo>
// dfti
#include "autopilot.hh"
#include "util/util.hh"


namespace dfti {


// ----------------------------------------------------------------------------
//  Constructors/destructors
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  Public functions
// ----------------------------------------------------------------------------

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
            if (_debug) {
                qDebug() << "dropped" << status.packet_rx_drop_count
                         << "packets";
            }
            lastStatus = status;
        }

        // Extract the message type if we have a full packet.
        if (msgReceived) {
            switch (message.msgid) {
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
                    if (_debug) {
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
                    if (_debug) {
                        qDebug() << "Autopilot::readData: SERVO_OUTPUT_RAW";
                    }
                    break;
                }
                default:
                    if (_debug) {
                        QString msgName = QString::number(message.msgid);
                        if (mavlinkMessageName.contains(message.msgid)) {
                            msgName = mavlinkMessageName[message.msgid];
                        }
                        qDebug() << "Got unhandled message type:"
                                 << msgName;
                    }
                    break;
            }
        }
    } else {
        if (_debug) {
            qDebug() << "Failed to read serial port!";
        }
    }

    // Emit message update if we have both, then reset.
    if (timestamps.rcChannelsRaw && timestamps.servoOutputRaw) {
        emit measurementUpdate(data);
        timestamps.reset();

        if (_debug) {
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

    return;
}

// ----------------------------------------------------------------------------
//  Private functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  Functions
// ----------------------------------------------------------------------------


};  // namespace dfti
