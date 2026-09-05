#ifndef LOG
#define LOG

#include <Arduino.h>
#include "config.h"
#include "bms_state.h"

const unsigned long logInterval = 5000;

void periodic_log(float, float, float, float, float, BatteryState, TemperatureState, GridState);

#endif