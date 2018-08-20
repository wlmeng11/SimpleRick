# SimpleRick

Low-cost 2D ultrasound imaging for makers

## Introduction
_“Come home to the impossible flavor of your own completion. Come home to Simple Rick's.”_

Based on kelu124’s [un0rick](http://un0rick.cc/), this project aims to provide hardware and software to perform 2D ultrasound imaging
at a cost affordable makers and electronics hobbyists.
To accomplish this, I’ve made a number of simplifications and modifications such as:
* removing the FPGA and its supporting circuitry
* using cheaper parts wherever possible
* reducing board size

Of course, these changes come at the expensive of flexibility and extensibility.
If all you want to do is get ultrasound images onto your computer as cheaply as possible, this project is for you.
If you want to do anything fancy like real-time processing, sophisticated motor control, integration with other sensors,
standalone capability, wireless operation, etc. then you’ll be better served by un0rick.

More to come soon!

## Links
* [Hackaday page](https://hackaday.io/project/160196-simplerick)
* Slack channel

## DISCLAIMER
This is NOT a medical device. Ultrasound technology can expose a user to hazards including but not limited to: high voltages, tissue-damaging acoustic waves, and materials containing lead (Pb). I am not liable for any injuries, death, or destruction of property that may occur as a result of using any materials provided by this project. You have been warned!

## LICENSE
The hardware in this project (located in subdirectory `hardware/`) is based on [un0rick](http://un0rick.cc/) (Copyright kelu124 2018),
and is similarly distributed under the [TAPR Open Hardware License](https://www.tapr.org/ohl.html).

The firmware in this project (located in subdirectory `firmware/`) is licensed under GPL v3.

The full text of each license can be found in the respective subdirectories.

Copyright (C) 2018 William Meng
