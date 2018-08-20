# SimpleRick PCB v1.1

| Field        | Value        |
|--------------|--------------|
| Version      | 1.1          |
| Author       | William Meng |
| Date         | 08/06/18     |
| Designed in  | Upverter     |
| License      | TAPR OHL     |
| Dimensions   | 2"x3"        |
| Layers       | 4            |
| Manufacturer | JLC PCB      |

Errata in PCB layout:
* swapped labels for V_PIEZO and GND testpoints
* LED_ACQUISITION should say LED_PWR
* some silkscreen labels are cutoff by vias

Things to be improved:
* high current traces should be wider
* need to add bleeder resistor between V_PIEZO and GND
* insufficient decoupling for servo motor
