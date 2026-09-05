#include "bms_state.h"
#include "config.h"

BatteryState batstate = BAT_NORMAL;
TemperatureState tempstate = TEMP_NORMAL;
GridState gridstate = NA;

void updateBMSState(float vbatt, float temp, float vrete) {
  // AGGIORNO STATO BATTERIA
  if(vbatt < VBATCRITICAL){
    batstate = BAT_CRITICAL;
  } else if(vbatt < VBATLOW){
    batstate = BAT_LOW;
  } else if(vbatt > VBATHIGH) {
    batstate = BAT_NORMAL;
  }

  // AGGIORNO STATO TEMPERATURA
  if(temp > TEMPCRITICAL){
    tempstate = TEMP_CRITICAL;
  } else if(temp > TEMPWARN){
    tempstate = TEMP_WARNING;
  } else if(temp < TEMPNORMAL) {
    tempstate = TEMP_NORMAL;
  }

  // AGGIORNO STATO RETE
  if(vrete < GRIDLOW){
    gridstate = OFFGRID;
  } else {
    gridstate = ONGRID;
  }
}
