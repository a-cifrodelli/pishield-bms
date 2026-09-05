#include "terminal.h"
#include "config.h"
#include "sensor.h"
#include "shutdown.h"

extern Shudown_Req shutdown_req;
extern bool cutoff_now;

void handleCommand(const char* cmd) {
  // Funzione di dispatch dei comandi: usa strcasecmp per case-insensitivity
  if (strcasecmp(cmd, "ACK") == 0) {
    if (shutdown_req.requested) {
      Serial.println(F("WAITING"));
      shutdown_req.cooldown = MIN_SHUTDOWN_COOLDOWN;
    }
  } 
  else if (strcasecmp(cmd, "SHUTDOWN_NOW") == 0) {
    requestUserShutdown();
  } 
  else if (strcasecmp(cmd, "PING") == 0) {
    if (SERIAL_DEBUG) Serial.println(F("PONG"));
  } 
#if TEST_MODE
  else if (strcasecmp(cmd, "SIM_BATT") == 0) {
    char* arg = strtok(NULL, " \t");
    if(arg) { mock_vbatt = atof(arg); Serial.print(F("SIM BATT: ")); Serial.println(mock_vbatt); }
  }
  else if (strcasecmp(cmd, "SIM_GRID") == 0) {
    char* arg = strtok(NULL, " \t");
    if(arg) { mock_vrete = atof(arg); Serial.print(F("SIM GRID: ")); Serial.println(mock_vrete); }
  }
  else if (strcasecmp(cmd, "SIM_TEMP") == 0) {
    char* arg = strtok(NULL, " \t");
    if(arg) { mock_temp = atof(arg); Serial.print(F("SIM TEMP: ")); Serial.println(mock_temp); }
  }
  else if (strcasecmp(cmd, "SIM_RESET") == 0) {
    mock_vbatt = MOCK_DEFAULT_VBATT;
    mock_vrete = MOCK_DEFAULT_VRETE;
    mock_temp = MOCK_DEFAULT_TEMP;
    shutdown_req.user_forced = false;
    Serial.println(F("SIM RESET: Dati normali di default ricaricati."));
  }
#endif
  else if (strcasecmp(cmd, "HELP") == 0) {
    Serial.println(F("--- COMMANDS ---"));
    Serial.println(F("ACK          : Acknowledge shutdown request"));
    Serial.println(F("SHUTDOWN_NOW : Force immediate shutdown"));
    Serial.println(F("PING         : Check connection"));
#if TEST_MODE
    Serial.println(F("SIM_BATT <v> : Simulate battery voltage (TEST_MODE)"));
    Serial.println(F("SIM_GRID <v> : Simulate grid voltage (TEST_MODE)"));
    Serial.println(F("SIM_TEMP <t> : Simulate temperature (TEST_MODE)"));
    Serial.println(F("SIM_RESET    : Restore default normal values"));
#endif
    Serial.println(F("HELP         : Show this message"));
    Serial.println(F("----------------"));
  } 
  else {
    Serial.print(F("UNKNOWN COMMAND: "));
    Serial.println(cmd);
  }
}

void handleSerial() {
  static char msg[32];      // buffer seriale fisso
  static size_t idx = 0;    // indice di scrittura

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {  // fine messaggio su newline o return
      if (idx > 0) {
        msg[idx] = '\0';
        
        // strtok è molto più pulito del trim manuale coi puntatori
        char* token = strtok(msg, " \t");
        
        if (token != NULL) {
          Serial.print(F("RX: "));
          Serial.println(token);
          handleCommand(token);
        }
        idx = 0;  // reset buffer
      }
    } 
    else if (idx < sizeof(msg) - 1) {
      msg[idx++] = c;
    }
  }
}
