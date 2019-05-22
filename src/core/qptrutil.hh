/*!
 *  \file qptrutil.hh
 *  \brief QPointer Utility functions.
 *  \author Joshua Harris
 *  \copyright Copyright © 2017 Vehicle Systems & Control Laboratory,
 *  Department of Aerospace Engineering, Texas A&M University
 *  \license BSD 2-Clause License
 *
 * This file is provided for instructional value only.  It is not guaranteed for any particular purpose.  
 * The authors do not offer any warranties or representations, nor do they accept any liabilities with respect 
 * to the information or their use.  This file is distributed with the understanding that the  authors are not engaged 
 * in rendering engineering or other professional services associate with their use.
 */
#pragma once


// Define macros to cast QPointers to their raw pointer types using Operator T*
// when TRAVISCI is defined, otherwise just use the QPointer.
#ifdef TRAVISCI
#define QSERIALPORTPTR(P) static_cast<QSerialPort *>(P)
#define QTHREADPTR(P) static_cast<QThread *>(P)
#define QTIMERPTR(P) static_cast<QTimer *>(P)
#define APPTR(P) static_cast<dfti::Autopilot *>(P)
#define LOGPTR(P) static_cast<dfti::Logger *>(P)
#define RIOPTR(P) static_cast<dfti::RIO *>(P)
#define SRVPTR(P) static_cast<dfti::Server *>(P)
#define UADCPTR(P) static_cast<dfti::uADC *>(P)
#define VN200PTR(P) static_cast<dfti::VN200 *>(P)
#else
#define QSERIALPORTPTR(P) P
#define QTHREADPTR(P) P
#define QTIMERPTR(P) P
#define APPTR(P) P
#define LOGPTR(P) P
#define RIOPTR(P) P
#define SRVPTR(P) P
#define UADCPTR(P) P
#define VN200PTR(P) P
#endif
