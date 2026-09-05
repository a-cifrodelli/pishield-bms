#include "shutdown.h"

// Riferimento esterno alla funzione in bms_logic.ino
extern void executeEmergencyCutoff(const __FlashStringHelper* reason);

void initRequestShutdown(){
  shutdown_req.requested = false;
  shutdown_req.can_interrupt_shutdown = false;
  shutdown_req.user_forced = false;
  shutdown_req.started_at = 0;
  shutdown_req.cooldown = MAX_SHUTDOWN_COOLDOWN;
  shutdown_req.cooldown_lastprint = 0;
}

void checkShutdownCooldown(){
  if(shutdown_req.can_interrupt_shutdown && shutdown_req.requested){
    initRequestShutdown();
  }
  else if(shutdown_req.requested) {
    serial_print_cooldown();
    if (millis() - shutdown_req.started_at >= shutdown_req.cooldown){
      initRequestShutdown();
      executeEmergencyCutoff(F("SHUTDOWN_TIMEOUT_EXPIRED"));
  	}
  }
}

void serial_print_cooldown(){
    unsigned long now = millis();
    unsigned long elapsed = now - shutdown_req.started_at;
    unsigned long remaining = (shutdown_req.cooldown > elapsed) ? (shutdown_req.cooldown - elapsed) : 0;
    
    // BUGFIX: mancava l'aggiornamento di cooldown_lastprint!
    if (now - shutdown_req.cooldown_lastprint >= 1000){
      Serial.print(F("SHUTDOWN IMMINENT - COOLDOWN: "));
      Serial.print(remaining);
      Serial.println(F(" ms"));
      shutdown_req.cooldown_lastprint = now; // Adesso stampa una sola volta al secondo
    }
}

void requestShutdown(){
  if(!shutdown_req.requested){
    Serial.println(F("SHUTDOWN_REQ"));
    shutdown_req.requested = true;
    shutdown_req.started_at = millis();
    shutdown_req.cooldown = MAX_SHUTDOWN_COOLDOWN;
  }
}

void requestUserShutdown(){
  if(!shutdown_req.requested){
    Serial.println(F("SHUTDOWN REASON: user_choice"));
    shutdown_req.requested = true;
    shutdown_req.user_forced = true;
    shutdown_req.started_at = millis();
    shutdown_req.cooldown = USER_CHOICE_SHUTDOWN_COOLDOWN;
    shutdown_req.can_interrupt_shutdown = false; // L'utente l'ha forzato, non si ferma da solo
  }
}
