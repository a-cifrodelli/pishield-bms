#include "log.h"

unsigned long lastLog = 0;

void periodic_log(float vbatt, float temp1, float temp2, float rete, float soc, BatteryState bs, TemperatureState ts, GridState gs){
// Log periodico
  if (millis() - lastLog >= logInterval) {
    Serial.print(F("[LOG] VBAT: "));
    Serial.print(vbatt, 2);
    Serial.print(F("V ("));
    Serial.print(soc, 1);
    Serial.print(F("%) | GRID: "));
    Serial.print(rete, 2);
    Serial.print(F("V ("));
    
    if(gs == ONGRID) Serial.print(F("ON"));
    else if(gs == OFFGRID) Serial.print(F("OFF"));
    else Serial.print(F("N/A"));
    
    Serial.print(F(") | TEMP: "));
    Serial.print(temp1, 1);
    Serial.print(F("C, "));
    Serial.print(temp2, 1);
    Serial.print(F("C | B:"));
    
    if (bs == BAT_NORMAL) Serial.print(F("OK"));
    else if (bs == BAT_LOW) Serial.print(F("LOW"));
    else if (bs == BAT_CRITICAL) Serial.print(F("CRIT"));
    
    Serial.print(F(" T:"));
    
    if (ts == TEMP_NORMAL) Serial.println(F("OK"));
    else if (ts == TEMP_WARNING) Serial.println(F("WARN"));
    else if (ts == TEMP_CRITICAL) Serial.println(F("CRIT"));

    lastLog = millis();
  }
}