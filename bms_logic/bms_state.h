#ifndef BMS_STATE_H
#define BMS_STATE_H

enum BatteryState { BAT_NORMAL, BAT_LOW, BAT_CRITICAL };
enum TemperatureState { TEMP_NORMAL, TEMP_WARNING, TEMP_CRITICAL };
enum GridState { ONGRID, OFFGRID, NA };

extern BatteryState batstate;
extern TemperatureState tempstate;
extern GridState gridstate;

void updateBMSState(float vbatt, float temp, float vrete);

#endif
