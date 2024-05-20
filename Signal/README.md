# Signal-Board

The board is designed with the Arduino Nano as a prototype. And the final design with Atmega328p which is the same in Arduino Nano Board.
The signals, which I use have 4 aspects, which are defined in the next enumeration outputs:

```c++
#define CON_1_PIN_R 4   // Pin 6  - PD4
#define CON_1_PIN_G 5   // Pin 11 - PD5
#define CON_1_PIN_Y 6   // Pin 12 - PD6
#define CON_1_PIN_O 7   // Pin 13 - PD7

#define CON_2_PIN_R 8   // Pin 14 - PB0
#define CON_2_PIN_G 9   // Pin 15 - PB1
#define CON_2_PIN_Y 10  // Pin 16 - PB2
#define CON_2_PIN_O 11  // Pin 17 - PB3

#define CON_3_PIN_R A5  // Pin 28 - PC5
#define CON_3_PIN_G A4  // Pin 27 - PC4
#define CON_3_PIN_Y A3  // Pin 26 - PC3
#define CON_3_PIN_O A2  // Pin 25 - PC1

#define CON_4_PIN_R A1  // Pin 24 - PD7
#define CON_4_PIN_G A0  // Pin 23 - PC0
#define CON_4_PIN_Y 13  // Pin 19 - PB5
#define CON_4_PIN_O 12  // Pin 18 - PB4PD7
```

and the DCC information, it takes from the pins:

```c++
#define DCC_ACK_SIGNAL 3  // Pin 5 - PD3
#define DCC_PIN_IN     2  // Pin 4 - PD2
```

# Sniffer-Signal

As arduino offers, it is programed in C++.
For the case od the DCC protocol, I it is used PADA (Port Accessory Decoder Address) configuration. Which means the next default configuration will be the one which makes the signals on:

```
Connector 1:
Address = <Address_1> ; Port = <Port_1> ; Gate = 0 ; ==> Aspect: Red
Address = <Address_1> ; Port = <Port_1> ; Gate = 1 ; ==> Aspect: Green
Address = <Address_1> ; Port = <Port_2> ; Gate = 0 ; ==> Aspect: Yellow
Address = <Address_1> ; Port = <Port_2> ; Gate = 1 ; ==> Aspect: Orange
```

```
Connector 2:
Address = <Address_1> ; Port = <Port_3> ; Gate = 0 ; ==> Aspect: Red
Address = <Address_1> ; Port = <Port_3> ; Gate = 1 ; ==> Aspect: Green
Address = <Address_1> ; Port = <Port_4> ; Gate = 0 ; ==> Aspect: Yellow
Address = <Address_1> ; Port = <Port_4> ; Gate = 1 ; ==> Aspect: Orange
```

```
Connector 3:
Address = <Address_2> ; Port = <Port_1> ; Gate = 0 ; ==> Aspect: Red
Address = <Address_2> ; Port = <Port_1> ; Gate = 1 ; ==> Aspect: Green
Address = <Address_2> ; Port = <Port_2> ; Gate = 0 ; ==> Aspect: Yellow
Address = <Address_2> ; Port = <Port_2> ; Gate = 1 ; ==> Aspect: Orange
```

```
Connector 4:
Address = <Address_2> ; Port = <Port_3> ; Gate = 0 ; ==> Aspect: Red
Address = <Address_2> ; Port = <Port_3> ; Gate = 1 ; ==> Aspect: Green
Address = <Address_2> ; Port = <Port_4> ; Gate = 0 ; ==> Aspect: Yellow
Address = <Address_2> ; Port = <Port_4> ; Gate = 1 ; ==> Aspect: Orange
```

And here are the DCC programming commands (Observation: it has to be in the programming track to make it work) are:

* For the case the default configuration, you have to send:

```
DCC Command: 8; Value = 0
```

* Address configuration

```
DCC Command: 37; Value = Address_1
DCC Command: 38; Value = Address_2
```

* Addressing Mode: Only allowed the PADA, until now.

```
DCC Command: 39; Value = 1
```

The possible values are:
```
Pada = 1
Fada = 2
```

Observation: The Fada (Flat Accessory Decoder Address) mode is not sported.

* Set the values on start-up, to set the values on start up, you can do with the commands:

```
DCC Command: 40; Value = 0
DCC Command: 42; Value = 0
```

The definition is:

For the DCC command 40, it covers:

```
Bits: x1-x2-x3-x4-x5-x6-x7-x8
    * x1-x2-x3-x4: Values associated to the connector 2
    * x5-x6-x7-x8: Values associated to the connector 1
```

For the DCC command 42, it covers:

```
Bits: x1-x2-x3-x4-x5-x6-x7-x8
    * x1-x2-x3-x4: Values associated to the connector 4
    * x5-x6-x7-x8: Values associated to the connector 3
```

* Define a blinking frequency:

```
DCC Command: 48; Value = 1
```

This frequency is set in seconds.

* Configure output for blinking:

```
Bits Value: y4-y3-y2-y1 x4-x3-x2-x1
    * y4-y3-y2-y1: which is the output has to blink
    * x4-x3-x2-x1: which of the 4 has to blink:
Example:
    * 0001 0010: Here will blink with the Port 1, the output G and the output R will be on.
    * 1001 0010: Here will blink with the Port 1 and Port 4, the output G and the output R will be on.
```

And the configuration are:

```
DCC Command: 44; Value = 0
DCC Command: 45; Value = 0
DCC Command: 46; Value = 0
DCC Command: 47; Value = 0
```

* Configure which output goes on, with which command:

The configuration for the four states is:

```
Default connector values
CV_CONNECTOR_X_COMMAND_1_2 = y4-y3-y2-y1 x4-x3-x2-x1 = 0x84
- Aspect 1: RGY0 XXXX = y4-y3-y2-y1
- Aspect 2: XXXX RGY0 = x4-x3-x2-x1
CV_CONNECTOR_X_COMMAND_3_4 = y4-y3-y2-y1 x4-x3-x2-x1 = 0x84
- Aspect 3: RGY0 XXXX = y4-y3-y2-y1
- Aspect 4: XXXX RGY0 = x4-x3-x2-x1
```

and the default definition is:

```c++
DCC Command: 49; Value = 0x84 // CV_CONNECTOR_1_COMMAND_1_2
DCC Command: 50; Value = 0x21 // CV_CONNECTOR_1_COMMAND_3_4
DCC Command: 51; Value = 0x84 // CV_CONNECTOR_2_COMMAND_1_2
DCC Command: 52; Value = 0x21 // CV_CONNECTOR_2_COMMAND_3_4
DCC Command: 53; Value = 0x84 // CV_CONNECTOR_3_COMMAND_1_2
DCC Command: 54; Value = 0x21 // CV_CONNECTOR_3_COMMAND_3_4
DCC Command: 55; Value = 0x84 // CV_CONNECTOR_4_COMMAND_1_2
DCC Command: 56; Value = 0x21 // CV_CONNECTOR_4_COMMAND_3_4
```
