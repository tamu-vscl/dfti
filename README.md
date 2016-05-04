# Developmental Flight Test Instrumentation

This repository contains the embedded firmware for the Developmental 
Flight Test Instrumentation (DFTI) developed by the Vehicle Systems & 
Control Laboratory. 
The DFTI package is a vehicle-agnostic microcontroller and sensor system 
that records vehicle states for use for system identification. 
Currently, the system is based on the Arduino Mega using the Atmel 2560 
microcontroller and interfacing with analog inputs, the VectorNav VN-200 
INS (RS232->UART), and the Aeroprobe Micro Air Data Computer 
(RS232->UART).

## Repo Organization

The main Arduino sketch is the `dfti.ino` file in the `src` directory.
Libraries provided with this sketch are in the `lib` directory, while
third-party libraries can be installed with the Platform IO tool. 
Currently the only third-party library required is AVRQueue, which should be 
automagically installed by PlatformIO.

## Build Targets

The following build targets are defined:

*   `default` -- Builds with support for the uADC and VN-200.
*   `vn200` -- Builds with support for *only* the VN-200.
*   `uadc` -- Builds with support for *only* the uADC.
*   `debug` -- Builds with support for both sensors and serial debugging.

Note that the analog inputs and PWM input are enabled for all targets. 
To select a built target, use
```
platformio run -e $TARGET
```
to build the project and 
```
platformio run -e $TARGET -t upload
```
to build and upload to the board.

