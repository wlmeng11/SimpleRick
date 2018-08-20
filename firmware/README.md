# SimpleRick firmware
TODO: travis badge http://docs.platformio.org/en/latest/ci/travis.html

This firmware is written for the Teensy 3.2 microcontroller, which should be connected to the SimpleRick PCB.

## Dependencies
Make sure Python 2.7 and pip are installed, then run:  
`pip2 install platformio`

## Usage
Build and upload program to Teensy:  
`platformio run --target upload`

## Features
#### TGC self-calibration
The heart of the Time Gain Control (TGC) circuit is an analog integrator, whose rate of integration
is determined by an RC time constant.
Due to variations in manufacturing and operating conditions, this time constant can vary considerably.
Therefore, the calibration procedure measures the TGC period and adjusts the input voltage
in order to achieve the desired TGC period.

#### JSON command interface (NOT YET IMPLEMENTED)
Thanks to the [ArduinoJSON](https://github.com/bblanchon/ArduinoJson) library,
image acquisition can be configured and initiated by JSON data sent to the Teensy's serial interface.


## License
The firmware contained herein is distributed according to the terms of the GNU General Public License version 3.
The full text of the license can be found in the [COPYING](COPYING) file.

Copyright (C) 2018 William Meng
