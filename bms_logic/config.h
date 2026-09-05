#ifndef CONFIG
#define CONFIG

// ===============================
// DISPLAY
// ===============================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// ===============================
// HARDWARE PINS
// ===============================
#define PINBATT A0
#define PINRETE A1
#define PINNTC1 A2   // NTC cella 1
#define PINNTC2 A3   // NTC cella 2
#define RELAYPIN 4    // relay sgancio batterie
#define RXPIN 8   // RX Arduino (collegato a TX RPi)
#define TXPIN 9   // TX Arduino (collegato a RX RPi)

// ===============================
// PARTITORI (da adattare ai tuoi valori)
// ===============================
const float R1 						= 10000.0;   // 10kΩ
const float R2 						= 10000.0;   // 10kΩ

// ===============================
// NTC 
// ===============================
const float R_FIXED 			    = 100000.0; // resistenza fissa 100k
const float BETA 					= 3950.0;      // parametro del datasheet
const float T0 						= 298.15;        // 25 °C in Kelvin
const float R0 						= 100000.0;      // resistenza NTC a 25°C

// ===============================
// SOGLIE DI SICUREZZA
// ===============================
const float VBATLOW					= 3.2;//3.3;   // V soglia warning
const float VBATHIGH     			= 3.7;//3.7;   // V isteresi riattacco
const float VBATCRITICAL 			= 3.0;   // V hard cutoff

const float TEMPWARN      			= 70.0;  // °C warning
const float TEMPCRITICAL  			= 75.0;  // °C hard cutoff
const float TEMPNORMAL          = 40;

const float GRIDLOW         = 4.0;

// Gli enumeratori di stato sono stati spostati in bms_state.h

// ===============================
// CONFIG FLAG
// ===============================
#define SERIAL_DEBUG true   // se true abilita PING/PONG e log verbosi
#define TEST_MODE false      // se true usa dati fittizi per i sensori (mock)
#if TEST_MODE
#define MOCK_DEFAULT_VBATT 4.0
#define MOCK_DEFAULT_VRETE 5.0
#define MOCK_DEFAULT_TEMP 25.0
#endif
#define GLOBAL_TICK 200

// ===============================
// COOLDOWN
// ===============================
const unsigned int MIN_SHUTDOWN_COOLDOWN = 30000;
const unsigned int MAX_SHUTDOWN_COOLDOWN = 60000;
const unsigned int USER_CHOICE_SHUTDOWN_COOLDOWN = 15000;

typedef struct Shudown_Req{
  bool requested;
  bool can_interrupt_shutdown;
  bool user_forced;
  unsigned long started_at;
  unsigned long cooldown;
  unsigned long cooldown_lastprint;
};

#endif