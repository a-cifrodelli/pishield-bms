#include "config.h"
#include "sensor.h"
#include "display.h"
#include "log.h"
#include "terminal.h"
#include "shutdown.h"
#include "bms_state.h"
Shudown_Req shutdown_req;
bool cutoff_now = false;

void executeEmergencyCutoff(const __FlashStringHelper* reason) {
  if (cutoff_now) return; // Evita di spammare la seriale se è già staccato
  
  digitalWrite(RELAYPIN, LOW); // SGANCIO FISICO IMMEDIATO
  cutoff_now = true;

  Serial.println(F("\n================================="));
  Serial.print(F("!!! EMERGENCY CUTOFF TRIGGERED !!!\nREASON: "));
  Serial.println(reason);
  Serial.println(F("=================================\n"));
}

// Le logiche di shutdown sono state spostate in shutdown.cpp

void setup() {
  Serial.begin(115200);
  delay(100); // Breve assestamento per la seriale

  Serial.println(F("\n================================="));
  Serial.println(F("   PISHIELD BMS - BOOTING...     "));
  Serial.println(F("================================="));
  
  Serial.println(F("[BOOT] Setup Pin I/O..."));
  pinMode(RELAYPIN, OUTPUT);
  pinMode(PINBATT, INPUT);
  pinMode(PINRETE, INPUT);
  pinMode(PINNTC1, INPUT);
  pinMode(PINNTC2, INPUT);
  
  Serial.println(F("[BOOT] Init Shutdown Logic..."));
  initRequestShutdown();
  
  Serial.println(F("[BOOT] Init OLED Display..."));
  initDisplay();
  
  Serial.println(F("[BOOT] Attivazione Rele' OUT..."));
  digitalWrite(RELAYPIN, HIGH);
  
  Serial.println(F("================================="));
  Serial.println(F("     SISTEMA PRONTO ALL'USO      "));
  Serial.println(F("================================="));

  delay(2000);
}

void loop() {
  // LETTURE SENSORI
  float vbatt = readBattVoltage();
  float soc = voltageToSOC(vbatt);
  float temp1 = readTemperature(PINNTC1);
  float temp2 = readTemperature(PINNTC2);
  float temp  = max(temp1, temp2); // prende la peggiore
  float vrete = readGridVoltage();

  // AGGIORNO STATO
  updateBMSState(vbatt, temp, vrete);

  // --- CUTOFF IMMEDIATI ---
  if(tempstate == TEMP_CRITICAL){
    executeEmergencyCutoff(F("TEMPERATURE_CRITICAL"));
  }
  if((batstate == BAT_CRITICAL) && (gridstate == OFFGRID)){
    executeEmergencyCutoff(F("BATTERY_CRITICAL_AND_OFFGRID"));
  }

  // --- SHUTDOWN RITARDATI (WARNING) ---
  if (!cutoff_now) {
    bool need_shutdown = false;
    if (tempstate == TEMP_WARNING) need_shutdown = true;
    if ((batstate == BAT_LOW) && (gridstate == OFFGRID)) need_shutdown = true;

    if (need_shutdown) {
      requestShutdown();
      shutdown_req.can_interrupt_shutdown = false; // L'allarme è attivo, spegnimento in corso
    } else {
      // Tutto regolare. Se c'era uno spegnimento in corso (non forzato dall'utente), annullalo.
      if (!shutdown_req.user_forced) {
        shutdown_req.can_interrupt_shutdown = true;
      }
    }
  }

  // AUTO-RECOVERY (Es. per Scenario 8 del piano di test)
  if (cutoff_now && gridstate == ONGRID && tempstate == TEMP_NORMAL && !shutdown_req.user_forced) {
     Serial.println(F("[RECOVERY] Rete ripristinata e temperatura sicura. Riattacco il rele'."));
     cutoff_now = false;
     digitalWrite(RELAYPIN, HIGH);
  }
  //SERIALE E COOLDOWN TIME-SPLICING
  checkShutdownCooldown();
  handleSerial();
  //LOG &DISPLAY
  updateDisplay(vbatt, temp1, temp2, vrete, soc, batstate, tempstate, gridstate);
  periodic_log(vbatt, temp1, temp2, vrete, soc, batstate, tempstate, gridstate);
  delay(GLOBAL_TICK);
}