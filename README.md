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
Currently the only third-party library required is AVRQueue, which has
PlatformIO ID 181, which can be installed with
```
$ platformio lib install 181
```
