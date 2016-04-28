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

The main Arduino sketch is the `dfti.ino` file.
Libraries provided with this sketch are in the `libs` directory, while
bundled third-party libraries are in the `third_party` directory. 
Note that these will generally need to be moved into the `Libraries` 
directory of your Arduino workspace for the Arduino IDE to find the 
libraries.
