/*!
 *  \file util.cc
 *  \brief Utility function implementations.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016-2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license BSD 2-Clause License
 *
 * This file is provided for instructional value only.  It is not guaranteed for any particular purpose.  
 * The authors do not offer any warranties or representations, nor do they accept any liabilities with respect 
 * to the information or their use.  This file is distributed with the understanding that the  authors are not engaged 
 * in rendering engineering or other professional services associate with their use.
 */
#include "util.hh"


namespace dfti {


quint64
getTimeUsec(void)
{
    static struct timeval _ts;
    gettimeofday(&_ts, nullptr);
    return 1e6 * _ts.tv_sec + _ts.tv_usec;
}


quint64
gpsToUnixUsec(quint64 gpsTime)
{
    const float nsToUs = 1e-3;
    const quint64 gpsEpochFromUnixEpochUs = 315964800000000;
    return static_cast<quint64>(gpsEpochFromUnixEpochUs + gpsTime * nsToUs);
}


quint64
gpsToUnixSec(quint64 gpsTime)
{
    const float usToSec = 1e-6;
    return static_cast<quint64>(gpsToUnixUsec(gpsTime) * usToSec);
}


float
hzToMsec(quint8 rate)
{
    return 1e3 / static_cast<float>(rate);
}


float
hzToUsec(quint8 rate)
{
    return 1e6 / static_cast<float>(rate);
}


float
secToMsec(quint8 period)
{
    return 1e3 * static_cast<float>(period);
}


};  // namespace dfti
