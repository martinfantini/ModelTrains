#pragma once
//File with general definitions of the parameters
#define DEBUG

//Maximum output current
#define BOOSTER_MAX_CURRENT 10.0

//Maximum temperature
#define BOOSTER_MAX_TEMPERATURE 80.0

// the two languagues posibilities are DE and EN
#define LANGUAGE_GERMAN

// the two possibles values
//#define ARDUINO_NANO

//MAximun converted value
#define POTNETIOMETER_MAX_VALUE     1024

// Sampling parameters
#define POTNETIOMETER_SAMPLES       100

// Temperature wating time.
#define TEMPERATURE_WAIT_TIME 1000

// Screen Refresh tiem time.
#define REFRESH_WAIT_TIME 500

// MAx Error Time
#define ERROR_WAIT_TIME 1000

// Sampling parameters
#define CURRENT_SAMPLES       400

#define STATUS_RUNNING        0
#define STATUS_STOP           1
#define STATUS_WAITING        2
#define STATUS_SHORT_CIRCUIT  3
#define STATUS_MAX_CURRENT    4
#define STATUS_EMERGENCY_STOP 5
