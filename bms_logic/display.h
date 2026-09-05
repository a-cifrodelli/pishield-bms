#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "config.h"
#include "bms_state.h"

// Struttura per gestire lo stato dello scroll di una singola riga
typedef struct ScrollLine {
  char text[32] ="";              // contenuto corrente
  int y = 0;                    // posizione verticale (pixel)
  int offset = 0;               // posizione scroll corrente
  unsigned long lastUpdate = 0; // ultimo tick di aggiornamento
};

const unsigned long scrollSpeed = GLOBAL_TICK; // velocità scroll in ms
const int CHAR_WIDTH = 6;             // font base 6x8
const int CHAR_HEIGHT = 8;
const int N_ROWS = 4;
const int INIT_RETRY = 20;

void initDisplay();
void setLineText(int index, int y, const char *newText);
void updateScrolling();
void updateDisplay(float, float, float, float, float, BatteryState, TemperatureState, GridState);

#endif