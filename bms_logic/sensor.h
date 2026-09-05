#ifndef SENSOR
#define SENSOR

#include <Arduino.h>
#include "config.h"

const int N_SAMPLES = 10;

const float voltages[] = {4.20, 4.10, 3.95, 3.85, 3.80, 3.75, 3.70, 3.60, 3.50, 3.40, 3.30};
const float soc[]      = {100,  90,   80,   70,   60,   50,   40,   30,   20,   10,    0};

#if TEST_MODE
extern float mock_vbatt;
extern float mock_vrete;
extern float mock_temp;
#endif

float readBattVoltage();
float readGridVoltage();
float readTemperature(int);
float analog_read_moving_average(int);
float voltageToSOC(float);

#endif