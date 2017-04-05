# Arduino RIO Firmware

This directory contains example Arduino firmware for a Remote I/O 
device. 
This particular firmware reads in control effector positions from 
linear potentiometers and the engine RPM from an RPM sensor and sends 
these data over serial using a simple protocol.

## Serial Protocol

Messages take the format
```
$$$value_1$value_2$...$value_n$checksum\r\n
```
where the packet start is indicated by the string `$$$`, values are 
separated by a single `$` character, with the last value being a 
checksum byte.
The packet is terminated with a carriage-return and line-feed, `\r\n`.

### Checksum

The checksum is very simple; it is simply the bitwise XOR of each byte
in the packet up to and including the separator immediately prior to the
checksum byte.
The checksum byte is written as a zero padded hexadecmimal value so that
the byte is always represented as two `char`s.

### Example

An example message is `$$$78$2$4$7$11$3e\r\n`.

## Building and Deploying the Firmware

The firmware can be compiled and uploaded to an Arduino board using the
Arduino IDE, or by using the [PlatformIO](http://platformio.org) 
development environment (recommended).

The PlatformIO build is configured in the `platformio.ini` file for a 
basic Arduino Uno build with `-Wall`; additional `CFLAGS` can be 
specified for a stricter build.

To build (compile) the firmware, run
```
platformio run
```
from the `firmware` directory.
To build and upload, run
```
platformio run -t upload
```
Additionally, a custom target can be specified by adding an `-e $TARGET`
argument, where `$TARGET` is a build target defined in the 
`platformio.ini` configuration file.

