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



