#include "sensor.h"

#if TEST_MODE
float mock_vbatt = MOCK_DEFAULT_VBATT;
float mock_vrete = MOCK_DEFAULT_VRETE;
float mock_temp = MOCK_DEFAULT_TEMP;
#endif

float readBattVoltage() {
#if TEST_MODE
  // Ritorna il valore impostato dai comandi + rumore (+-0.1V)
  return mock_vbatt + ((random(100) - 50) / 500.0);
#endif
  float raw = analog_read_moving_average(PINBATT);  
  float vAdc = (raw / 1023.0) * 5.0;  // tensione misurata dal pin
  float vBatt = vAdc * ((R1 + R2) / R2);  // formula partitore
  return vBatt;
}

float readGridVoltage() {
#if TEST_MODE
  // Ritorna il valore impostato + rumore (+-0.3V)
  return mock_vrete + ((random(100) - 50) / 166.0);
#endif
  float raw = analog_read_moving_average(PINRETE);
  float vAdc = (raw / 1023.0) * 5.0;
  float vRete = vAdc * ((R1 + R2) / R2);
  return vRete;
}

float readTemperature(int pinNTC) {
#if TEST_MODE
  // Ritorna il valore impostato + rumore (+-1.0 gradi)
  return mock_temp + ((random(100) - 50) / 50.0);
#endif
  float raw = analog_read_moving_average(pinNTC);
  
  // FAILSAFE: Sensore scollegato o in corto
  if (raw <= 0 || raw >= 1023) return 999.0;

  float v = (raw / 1023.0) * 5.0;   // tensione ADC
  float Rntc = (R_FIXED * (5.0 / v - 1.0));

  float steinhart;
  steinhart = log(Rntc / R0) / BETA;   // (ln(R/R0))/B
  steinhart += 1.0 / T0;               // + (1/T0)
  steinhart = 1.0 / steinhart;         // inverti
  steinhart -= 273.15;                 // Kelvin → °C

  return steinhart;
}

float analog_read_moving_average(int pin) {
  long sum = 0;
  for (int i=0; i < N_SAMPLES; i++){
    sum += analogRead(pin);
    delay(2);
  }
  return sum / (float) N_SAMPLES;
}

// SOC da tensione singola cella Li-ion (valido anche con più celle in parallelo)
float voltageToSOC(float vcell) {
  const int size = sizeof(voltages) / sizeof(voltages[0]);

  if (vcell >= voltages[0]) return 100.0;
  if (vcell <= voltages[size-1]) return 0.0;

  // interpolazione lineare tra due punti della curva
  for (int i = 0; i < size-1; i++) {
    if (vcell <= voltages[i] && vcell > voltages[i+1]) {
      float m = (soc[i+1] - soc[i]) / (voltages[i+1] - voltages[i]);
      return soc[i] + m * (vcell - voltages[i]);
    }
  }

  return 0.0; // fallback (non dovrebbe mai capitare)
}