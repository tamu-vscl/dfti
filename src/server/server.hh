/*!
 *  \file server.hh
 *  \brief Data server class interface.
 *  \author Joshua Harris
 *  \copyright Copyright © 2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license BSD 2-Clause License
 */
#pragma once


// stdlib
#include <vector>
// 3rd party
#include <QDebug>
#include <QHostAddress>
#include <QObject>
#include <QPointer>
#include <QProcess>
#include <QTimer>
#include <QUdpSocket>
// dfti
#include "core/consts.hh"
#include "rio/rio.hh"
#include "settings/settings.hh"
#include "uadc/uadc.hh"
#include "util/util.hh"
#include "vn200/vn200.hh"


#define STATE_DATA_SIZE 10


namespace dfti {


/*! \brief Structure to hold state data published.
 *
 *  For online system identification and similar use cases, we need the vehicle
 *  state data available. This data structure holds a minimum set of state data
 *  as a POD struct with 1 byte structure packing.
 *
 *  State data comes from the INS, ADS, and control effector RIOs. If these
 *  sensors are inactive values of zero are used.
 *
 *  The StateData structure is assumed to use the native byte order.
 *
 *  \note Since DFTI is designed such that the number of RIO values is
 *  variable, the StateData struct assumes that there are at most 10 values and
 *  preallocates a float array accordingly. An unsigned char is then used to
 *  indicate to the user how many of these values are actually used. Further, it
 *  is the user's responsibility to use these values correctly.
 *
 */
#pragma pack(push, 1)  // change structure packing to 1 byte
struct StateData
{
    //! INS GPS timestamp.
    quint64 gpsTimeNs{0};

    //! INS Euler angles.
    float eulerDeg[3] = {0};

    //! INS quaternion.
    float quaternion[4] = {0};

    //! INS angular rates.
    float angularRatesRPS[3] = {0};

    //! INS accelerations.
    float accelMps2[3] = {0};

    //! ADS indicated airspeed.
    float iasMps{0};

    //! ADS angle-of-attack.
    float aoaDeg{0};

    //! ADS sideslip angle.
    float aosDeg{0};

    //! Number of RIO values (up to 10).
    quint8 numRIOValues{0};

    //! RIO values.
    float rioValues[STATE_DATA_SIZE] = {0};
};
#pragma pack(pop)  // reset structure packing


/*! \brief UDP server for vehicle state data.
 *
 *  For online system identification and similar use cases, we need the vehicle
 *  state data available. This class implements a UDP server that server the
 *  StateData structure at a user-specified rate. The data is sent to a
 *  user-specified IP address and port; these default to localhost and 2701.
 *
 *  The native byte order and 1-byte padding is used for the structure; no
 *  conversions are made to network byte order. The easiest way to receive the
 *  data is to bind a socket to the address and port and cast the bytes to the
 *  data structure; in C++ static_cast<StateData> should work.
 *
 *  For Python, code similar to the following may be used:
 *  \code{.py}
 *  import socket
 *  import struct
 *
 *  BUF_SIZE = 128  # Make sure this is larger than sizeof(StateData)!
 *  SOCK_ADDR = "127.0.0.1"
 *  SOCK_PORT = 2701
 *
 *  if __name__ == '__main__':
 *      sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
 *      sock.bind((SOCK_ADDR, SOCK_PORT))
 *
 *      fmt = '=QffffffffffffffffBffffffffff'
 *      while True:
 *          data, _ = recvfrom(BUF_SIZE)
 *          unpacked_data = struct.unpack(fmt, data)
 *          # Do stuff with unpacked_data, which is a tuple.
 *  \endcode
 *
 */
class Server : public QObject
{
    Q_OBJECT;

public:
    //! Constructor
    /*!
     *  \param _settings Pointer to Settings object.
     *  \param _parent Pointer to parent QObject.
     */
    explicit Server(Settings *_settings, QObject* _parent = nullptr);

    //! Dtor.
    ~Server();

    //! Enable Micro Air Data Computer Sensor.
    /*!
     * \param adc QPointer to uADC object.
     */
    void enableUADC(uADC *adc);

    //! Enable Remote I/O unit.
    /*!
     * \param rio QPointer to RIO object.
     */
    void enableRIO(RIO *rio);

    //! Enable VN-200 INS Sensor.
    /*!
     * \param ins QPointer to VN200 object.
     */
    void enableVN200(VN200 *ins);

    //! Start server.
    /*!
     *  Connects QTimers to the writeData slot.
     */
    void start(void);

public slots:
    //! Slot to receive data from the RIO.
    void getRIOData(RIOData data);

    //! Slot to receive data from the Micro Air Data Computer.
    void getUADCData(uADCData data);

    //! Slot to receive data from the VN-200 INS.
    void getVN200Data(VN200Data data);

    //! Slot to write data.
    void writeData(void);

private:
    //! Pointer to settings object.
    QPointer<Settings> settings{nullptr};

    //! UDP socket.
    QPointer<QUdpSocket> socket{nullptr};

    //! UDP server address, default localhost.
    QHostAddress address{QHostAddress::LocalHost};

    //! UDP port, default 2701.
    quint16 port{2701};

    //! QTimer for writing.
    QTimer *writeTimer{nullptr};

    //! Server state data structure.
    StateData stateData;
};


};  // namespace dfti
