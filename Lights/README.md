# Lights

For this modele, I use the [SBC-USR-ES1 module](./SBC-USR-ES1_MANUAL_2021-04-22.pdf) and the Arduino AT Mega 328.

## Lights-Shild

Here is the circuit board of this project.

## ArtnetReceiver

Here is the code of the Lights-Shild, using the [SBC-USR-ES1 module](./SBC-USR-ES1_MANUAL_2021-04-22.pdf) module.
The IP definied for this module is start and you have to change in code:

```c
//DEFINE THE IP HERE WICH HEARS ARNET MESSAGES
byte ip[] = {192, 16, 178, 205};
```
