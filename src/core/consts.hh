/*!
 *  \file consts.hh
 *  \brief DFTI constants.
 *  \author Joshua Harris
 *  \copyright Copyright © 2016 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license ISC License
 */
#pragma once


namespace dfti {


//! DFTI application name.
const QString app_name{"dfti"};

//! DFTI application version.
/*!
 *  \remark DFTI uses semantic versioning with the major, minor, and patch
 *      versions defined as macros in the main CMakeLists.txt file.
 */
const QString app_version{DFTI_VERSION};

//! Available sensors enumeration.
enum class AvailableSensors : quint8 {
    NONE       = 0,       /// No sensors present
    HAVE_AP    = 1 << 0,  /// MAVLink-based autopilot present
    HAVE_UADC  = 1 << 1,  /// Micro Air Data Computer present
    HAVE_VN200 = 1 << 2   /// VN-200 INS present
};


//! Debugging Mode enumeration
enum class DebugMode : quint8
{
    DEBUG_NONE   = 0,       /// Don't display debug messages
    DEBUG_RC     = 1 << 0,  /// Display settings debug messages
    DEBUG_SERIAL = 1 << 1,  /// Display serial i/o debug messages
    DEBUG_DATA   = 1 << 2   /// Display received data
};


// -----------------------------------------------------------------------------
//  Operator Overloading for Enum Classes
// -----------------------------------------------------------------------------

//! Implement bitwise-or for AvailableSensors
inline AvailableSensors
operator|(AvailableSensors lhs, AvailableSensors rhs)
{
    return static_cast<AvailableSensors>(
        static_cast<quint8>(lhs) | static_cast<quint8>(rhs)
    );
}


//! Implement bitwise-or assignment for AvailableSensors
inline AvailableSensors&
operator|=(AvailableSensors& lhs, AvailableSensors rhs)
{

    lhs = static_cast<AvailableSensors>(
        static_cast<quint8>(lhs) | static_cast<quint8>(rhs)
    );
    return lhs;
}


//! Implement bitwise-and for AvailableSensors
inline AvailableSensors
operator&(AvailableSensors lhs, AvailableSensors rhs)
{
    return static_cast<AvailableSensors>(
        static_cast<quint8>(lhs) & static_cast<quint8>(rhs)
    );
}


//! Check an AvailableSensors value.
inline bool
check(AvailableSensors x)
{
    return static_cast<quint8>(x) ? true : false;
}


//! Implement bitwise-or for DebugMode
inline DebugMode
operator| (DebugMode lhs, DebugMode rhs)
{
    return static_cast<DebugMode>(
        static_cast<quint8>(lhs) | static_cast<quint8>(rhs)
    );
}


//! Implement bitwise-or assignment for DebugMode
inline DebugMode&
operator|=(DebugMode& lhs, DebugMode rhs)
{

    lhs = static_cast<DebugMode>(
        static_cast<quint8>(lhs) | static_cast<quint8>(rhs)
    );
    return lhs;
}


//! Implement bitwise-and for DebugMode
inline DebugMode
operator& (DebugMode lhs, DebugMode rhs)
{
    return static_cast<DebugMode>(
        static_cast<quint8>(lhs) & static_cast<quint8>(rhs)
    );
}


//! Check a DebugMode value.
inline bool
check(DebugMode x)
{
    return static_cast<quint8>(x) ? true : false;
}


};  // namespace dfti
