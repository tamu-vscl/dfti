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

### Bill Of Materials 

Please note that any of the following components can be replaced with a similar component as long as it is compatible and a custom sensor module is written to parse the data (if applicable). In other words, different brands or types of sensors may be used as long as they're integrated properly. There is no dependence of the system to use these particular products.

* 1 x BeagleBone Black
* 1 x BeagleBone Black Protoshield
* 1 x Arduino
* 1 x Arduino Protoshield
* 4 x Servo Y-harness
* 1 x Airspeed sensor
  * Aeroprobe Micro Air Data Computer
* 1 x Throttle sensor
  * EagleTree Brushless Motor RPM Sensor
* 4 x Male to Male Servo Extensions
* 1 x Inertial Navigation System 
  * VectorNav VN-200
* 1 x INS Connector Cable 
  *  VN-C200-0610 Cable
* 1 x Logic-Level Bidirectional Voltage Shifter
* Connectors:
  * HSG Female 3POS 2.5mm Post Lock
  *	HSG Female 4POS 2.5mm Post Lock
  *	CONN Header Vert 3POS 2.5mm
  *	CONN Header Vert 4POS 2.5mm


## License

DFTI is placed under the [BSDv2 license](./LICENSE.md).

# Setup / Quick Start Guide
This document provides a guideline for setting up DFTI for the first time. This guide is intended to walk a new developer through the installation of any needed software and compiling of the DFTI code. A separate guide can be found below explaining the hardware and software architecture, as well as the functionality of the system.

## DFTI Setup:
### Install Ubuntu Virtual Machine
It is recommended to use an Ubuntu virtual machine for development. If you already have a computer running Ubuntu then skip ahead.

For a more detailed setup guide see [here](http://www.psychocats.net/ubuntu/virtualbox).

To set up the virtual machine, follow these steps.

1. Install [VirtualBox](https://www.virtualbox.org/) (mac users may need [VMWare](https://www.vmware.com/products/fusion.html).
2. Download [Ubuntu](https://www.ubuntu.com/download/desktop) iso.
3. Create new virtual machine with Ubuntu OS.

### Set up DFTI and Dependencies
1. Install git
  1. Open a terminal and run the command: `sudo apt-get install git`
2. Clone DFTI repo: https://github.tamu.edu/vscl/dfti.git 
  1. Use `git clone` command
3. Install Qt libraries. These are dependencies for DFTI.
  1. Run the following two commands:
    * `sudo apt-get install qtbase5-dev`
    * `sudo apt-get install libqt5serialport5-dev`
4. Install mavlink v1 
  1. Clone mavlink repo: https://github.com/mavlink/c_library_v1.git into its own directory (totally separate from DFTI).
  2. Copy contents of ‘c_library_v1/’ directory into ‘dfti/ext/mavlink/v1/’ directory.
5. From the root dfti directory (i.e. dfti/ ) run the command: `cmake CMakeLists.txt`
6. From the root dfti directory run the command:` make`

The project should build successfully. There should now be two executables under the dfti/bin/ directory, dfti and dftitest.

## Running dftitest
Dftitest is a separate process that is designed to test individual components of the system. This allows us to connect to a single component (for example, the Arduino RIO) and test it in isolation from the rest of the system.

### Running the program
To run dftitest navigate to the dfti/bin/ directory and run the following command with the [bracketed] text replaced by the appropriate options:

`./dftitest [sensor] [serial port] --config [config file]`

### Options
* The [sensor] option dictates which sensor you will be connecting to. The valid options are: ap, rio, uadc, vn200.
* The [serial port] options is required to indicate which serial port the sensor is connected to. Example: “/dev/ttyUSB0”
* The [config file] option must immediately follow the --config or -c command (either will work). This indicates the configuration file for the program, which must be in the same directory as the executable or you must provide a direct path.

### Example
`./dftitest vn200 /dev/ttyUSB0 --config test.ini`

## Installation
DFTI is not run as an executable, but rather installed in the OS as a package. This can be done using cmake by entering the command:

`sudo cmake install`

In the root DFTI direcotory.

NOTE: For Ubuntu Linux, installation may be placing the libraries in the wrong directory (/usr/lib64/ instead of /usr/lib/). This can be fixed by manually going into each of the cmake_install.cmake files in each sub-directory of the repo and changing all instances of “lib64” to “lib”. THIS NEEDS TO HAPPEN EVERY TIME YOU RE-RUN CMAKE.

This has to do with the CMakeLists.txt file. It sends them to lib or lib64 based on system architecture. Please note that they will have to be sent to /usr/lib64 when compiled for the BeagleBone Black.

## Cross Compiling
DFTI is typically developed in Ubuntu Linux. However, it runs on a BeagleBone Black using Debian Linux running on an ARM architecture. This causes problems when trying to compile, as you cannot (easily) install the dependencies in Debian needed to build from source. Nor can you compile directly on your dev machine and copy over the binaries. For this reason, you must perform a cross-platform compilation. The instructions to do so are as follows:

* Install the gcc and g++ arm compilers.
  * `sudo apt-get install gcc-arm*`
  * `sudo apt-get install g++-arm*`
* Install the dependency packages (Qt) for arm
  * Add arm architecture packages to apt-get
    *  Run the command `sudo dpkg --add-architecture armhf`
  * Updated sources.list file
    * `sudo gedit /etc/apt/sources.list`
    * For each non-commented line that lists a url (something about archive.ubuntu.com or something), add beneath it the exact same line with the url “http://ports.ubuntu.com/ubuntu-ports”
  *  Run `sudo apt-get update` and confirm that there are no errors

* At this point, we have installed all the necessary software to perform cross-compiling, we now need to perform the compilation
* Return to the dfti base directory and delete the CMakeCache.txt file
* Run `cmake -DCMAKE_TOOLCHAIN_FILE=config/armhf.cmake`
* Run `sudo make install`
  * This will compile the code with the newly installed compiler and arm libraries
* Now, we have successfully, compiled the code, but we still need to copy over the files to the beaglebone.
  * Connect to the beaglebone via ssh
  * Create a temporary directory to store the files
  * Use rsync to transfer over the following files to the temp directory. Then use mv to move them to the given location (format is dev_machine_file_location -> bb_file_location):
    * /usr/bin/dfti -> /usr/bin
    * /usr/lib/libdfti* -> /usr/lib
    * /usr/lib/arm-linux-gnueabihf/lib* -> /usr/lib
    * Your .ini file, wherever you keep that -> wherever you want to keep it on the bb
* You should now be able to run dfti by the command:
  * `dfti -c your_config_file.ini`


### Debugging resources:
https://wiki.debian.org/Multiarch/HOWTO
https://cmake.org/cmake/help/v3.6/manual/cmake-toolchains.7.html 

## Running
1. Install bb cape overlays (this was about impossible to find, so it’s possible that this may/used to come standard and my bbb just didn’t have it for some reason).
2. On the BeagleBone (connected to the internet via an ethernet cable):
3. Run `sudo sudo apt update`
4. Run `sudo apt install bb-cape-overlays`



## Running DFTI
1. Connect to BBB via SSH
2. Navigate to bin directory
3. Run `tmux new -s dfti`
4. In the tmux window, run `sudo dfti -c test.ini`
  * If test.ini is not in this directory (or any ini) you need to place it here (or any valid ini file)
5. In the tmux window, `ctrl+b d`
6. Disconnect from ssh

## Reconnecting and shutting down:
1. Conect to BBB via ssh
2. Run `tmux ls` to make sure dfti is still running
3. If it is, run `tmux a -t dfti`
4. Use `ctrl+c` to stop dfti
5. Run `exit` to kill tmux window
6. Use `exit` to kill ssh window
7. Use `rsync -r debian@192.168.7.2:bin/ destination/` to copy all the files from the BBB bin directory to the destination directory
Done

# DFTI Architecture

This section provides an explanation of the system architecture for DFTI. The intent is to provide a new developer with a document that explains, in general, how the system works, what the different modules are, and how they interact with one another. Additionally, this document will provide a brief explanation of some of the supporting libraries used (i.e. Qt). 

## Overview
In general, the system consists of a data logger, server, and (presently) four serial sensors. Each module runs in a separate thread. The serial sensors collect data from the various sensors and report that data back to the logger and server. The logger simply writes out all incoming sensor data into a csv file. The server acts as a UDP server which will serve up the latest data.

## Software Architecture
The software architecture consists of a logger and server, which each independently communicate with each of the sensor modules. This communication is carried out by using signals and slots provided by the Qt library. Each of the sensor modules communicates with the actual hardware sensors via serial ports (by use of the Qt serial port class). 

This architecture is shown below.

![picture](https://github.com/tamu-vscl/dfti/blob/master/SoftwareArchitecture.png)

## Sensor Modules
The sensor modules each provide an abstraction of the communication with the actual hardware sensor. The sensor modules are responsible for establishing connection to a sensor through a serial port (configurable in the config file). The sensor modules read incoming data from their respective sensor, parse the data into a known structure, and emit a signal to the logger and server containing the processed data. 

### Server
The server’s job is to keep track of the most recent data from each of the sensors and to provide this data at a specified rate over a UDP connection. It does this by using a socket to write data to a specified address and port. The address, port, and reporting rate are all configurable via the config file. 

The server has slots that respond to the corresponding signals from each one of the sensors. When a sensor emits a signal, the server receives it in the correct slot and updates the state data. This state data is then sent to the client at the specified reporting rate. 

### Logger
The logger behaves in a similar manner to the server. It has slots corresponding to the sensor signals. Upon receiving new data from one of the sensors, it updates its own local data with the most recent data. That is, the logger keeps a single data structure and only updates a particular data field whenever a sensor provides the logger with new data for that field. 

The differences though is that the logger logs out the data to a CSV rather than sending it over a UDP connection. The logger does NOT log out data as the sensors read it in. Instead, the logger runs on a timer and at specified time intervals logs out its current local data. 

### Class Hierarchy 
All of the classes inherit from QObject. This is what allows the different modules to communicate via signals and slots. The serial sensor class abstracts away much of the generic communication needed by each of the sensors. This allows them to communicate with the logger and server in the same generic way.

![picture](https://github.com/tamu-vscl/dfti/blob/master/ClassHierarchy.png)

## Hardware Architecture
The hardware for DFTI currently consists of a BeagleBone Black, an Arduino Uno, a VN-200, five feedback servos, and an rpm sensor. The DFTI software runs on the BeagleBone Black and reads data from the VN-200 directly via serial communication. The feedback servos and rpm sensor both emit analog signals and thus, are connected to the Arduino which reads in the analog signals, converts them into a digital format, and sends the data to the DFTI software on the BeagleBone. Again, this is done via serial  (UART) communication. 

### BeagleBone Stack
The hardware stack containing the BeagleBone (BB) is fairly straightforward. It consists of the actual BeagleBone board, a USB shield (cape in BB terminology), and a protoshield. The USB shield may actually not be necessary, as I have not yet found anything that needs to attach to it. For this reason, it is left off of the current DFTI stack in order to reduce weight and volume. The protoshield however, contains all of the connectors that connect the sensors to the BB’s UART ports and is very much necessary as all communication goes through these ports.

The setup for the BB stack is simply the protoshield stacked on top of the BB. However, the wiring of the protoshield is a bit more complex and is outlined below.

#### Protoshield Wiring
The protoshield basically just provides easy, plug-in connectors for our serial communication. The sensors have custom made cables that plug directly into the connectors on the shield. 

Pictures of the protoshield can be found below.

(https://github.com/tamu-vscl/dfti/blob/master/BBShieldFront.JPG )
(https://github.com/tamu-vscl/dfti/blob/master/BBShieldBack.jpg)

Formal Wiring Schematic:
![picture](https://github.com/tamu-vscl/dfti/blob/master/WiringSchematic.PNG)

The BB communicates with the sensors over serial communication using UART protocol. 

Each of the connectors has Vcc connected to the leftmost pin and ground connected to the rightmost. The middle two pins are the Rx and Tx pins of the UART communication. Ideally, the left should be connected to the BB Tx pin and the right to the BB Rx pin. 

For example, the connector for UART1 (third from the top in the picture) has pin 24 (BeagleBone Tx) connected to the middle-left pin of the connector and pin 26 (BeagleBone Rx) to the middle-right.

Note: The UART4 connector (top in picture) has the Rx and Tx pins soldered backward according to the above description.

The important point is not so much that the Tx and Rx pins go to the left and right pins of the connector, but instead that the BB Tx pin connects to the sensor’s Rx wire and the BB’s Rx pin to the sensor’s Tx. I attempted to standardize all of our cables so that the sensor’s Rx goes to the left pin in the connector and the sensor’s Tx to the right.


Pinout for BeagleBone Black can be found online. Note the serial pins, these are the ones we’re using.

#### Arduino Protoshield Schematic

Formal Wiring Schematic:
![picture](https://github.com/tamu-vscl/dfti/blob/master/ArduinoWiringSchematic.PNG)




