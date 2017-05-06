# Developmental Flight Test Instrumentation v2 

[![License](https://img.shields.io/badge/License-BSD%202--Clause-blue.svg?style=flat-square)](./LICENSE.md)
[![TravisCI Status](https://img.shields.io/travis/tamu-vscl/dfti/master.svg?style=flat-square)](https://travis-ci.org/tamu-vscl/dfti)
[![Coverity Scan Status](https://img.shields.io/coverity/scan/12636.svg?style=flat-square)](https://scan.coverity.com/projects/tamu-vscl-dfti)
[![Issues](http://img.shields.io/github/issues/tamu-vscl/dfti.svg?style=flat-square)](https://github.com/tamu-vscl/dfti/issues)
[![Release](https://img.shields.io/github/release/tamu-vscl/dfti.svg?style=flat-square)](https://github.com/tamu-vscl/dfti/releases)
[![DOI](https://zenodo.org/badge/90431398.svg?style=flat-square)](https://zenodo.org/badge/latestdoi/90431398)

The VSCL Developmental Flight Test Instrumentation (DFTI) is an embedded 
software package that handles flight test data recording for Small UAS 
flight test.
DFTI is a vehicle-agnostic package that can be easily attached to 
arbitrary SUAS, provided that the vehicle is large enough to carry the 
hardware.

This is Version 2 of DFTI, which replaces the Arduino-based Version 1 
firmware.
This latter version is kept in an archive branch `arduino`.

## Hardware

DFTI is primarily developed for use with the BeagleBone Black 
single-board computer, but should work with any recent embedded Linux 
that supports interfacing over UART.

### Sensors

DFTI is designed to record vehicle states for parameter and system
identification and does so via dedicated sensors to obtain air data, 
inertial measurements, and control deflections.
The current supported sensors are:

*   Air Data: Aeroprobe Micro Air Data Computer (RS-232)
*   INS: VectorNav VN-200 INS (UART/RS-232/SPI)
*   Control Surface Deflections: linear potentiometers via Arduino RIO*
*   Engine RPM: EagleTree Brushless RPM Sensor V2 via Arduino RIO

(* RIO = Remote Input/Output module)

Code exists to interface with MAVLink-based autopilots, but it is not
production-ready, and is still very much a work in progress.

DFTI is designed to be modular, so support for additional sensors may
be added easily.

## License

DFTI is placed under the [BSDv2 license](./LICENSE.md).

