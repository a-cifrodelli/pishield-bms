#include "display.h"

// ===============================
// DISPLAY SCROLLING LINE
// ===============================
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
ScrollLine lines[N_ROWS];   // fino a 4 righe (128x32 display → 8px per riga)

void initDisplay(){
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (int i=0;i<INIT_RETRY;i++){
      Serial.println("init loop");
    } // se non trova display → loop
  }
  for (int i = 0; i < N_ROWS; i++) {
    lines[i].text[0] = '\0';   // inizializza stringa vuota
    lines[i].offset = 0;
    lines[i].lastUpdate = 0;
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}


// ===============================
// SETTA UNA RIGA
// ===============================
void setLineText(int index, int y, const char *newText) {
  if (strncmp(newText, lines[index].text, sizeof(lines[index].text)) != 0) {
    // copia sicura in buffer
    strncpy(lines[index].text, newText, sizeof(lines[index].text) - 1);
    lines[index].text[sizeof(lines[index].text) - 1] = '\0'; // forza terminatore
    lines[index].offset = 0;
    lines[index].y = y;
    lines[index].lastUpdate = millis();
  }
}

// ===============================
// AGGIORNA CON LO SCROLL
// ===============================
void updateScrolling() {
  display.clearDisplay();

  for (int i = 0; i < N_ROWS; i++) {
    if (lines[i].text[0] == '\0') continue; // riga vuota → skip

    int textWidth = strlen(lines[i].text) * CHAR_WIDTH;

    // Cancella solo la banda verticale della riga
    display.fillRect(0, lines[i].y, SCREEN_WIDTH, CHAR_HEIGHT, SSD1306_BLACK);

    if (textWidth <= SCREEN_WIDTH) {
      // Testo entra nello schermo → stampa fisso
      display.setCursor(0, lines[i].y);
      display.print(lines[i].text);
    } else {
      // Testo più lungo → scroll continuo a loop circolare
      if (millis() - lines[i].lastUpdate > scrollSpeed) {
        lines[i].offset++;
        // Quando l'ultimo carattere esce completamente → ricomincia da 0
        if (lines[i].offset >= textWidth) {
          lines[i].offset = 0;
        }
        lines[i].lastUpdate = millis();
      }

      int x = -lines[i].offset;
      display.setCursor(x, lines[i].y);
      display.print(lines[i].text);

      // Disegna una seconda copia subito dopo la prima per creare il loop continuo
      display.setCursor(x + textWidth, lines[i].y);
      display.print(lines[i].text);
    }
  }

  display.display();
}



// ===============================
// DISPLAY HANDLER
// ===============================
void updateDisplay(float vbatt, float temp1, float temp2, float rete, float soc, BatteryState bs, TemperatureState ts, GridState gs) {
  char buffer[32];   // buffer principale
  char tmp[8];

  // Riga 0
  int rete_int = (int)(rete*10);
  snprintf(tmp, sizeof(tmp), "%d.%d", rete_int/10, rete_int%10);
  int vbatt_int = (int)(vbatt*10);
  snprintf(buffer, sizeof(buffer), "rete: %sV VBatt: %d.%dV", tmp, vbatt_int/10, vbatt_int%10);
  setLineText(0, 0, buffer);

  // Riga 1
  int t1_int = (int)(temp1*10);
  snprintf(tmp, sizeof(tmp), "%d.%d", t1_int/10, abs(t1_int%10));
  int t2_int = (int)(temp2*10);
  snprintf(buffer, sizeof(buffer), "T1: %sC T2: %d.%dC", tmp, t2_int/10, abs(t2_int%10));
  setLineText(1, 8, buffer);
    
  snprintf(buffer, sizeof(buffer), "Bat: %s Rete: %s ",
       (bs==BAT_NORMAL)?"NORMAL":(bs==BAT_LOW)?"LOW":(bs==BAT_CRITICAL)?"CRITICAL":"N/A",
       (gs==ONGRID)?"ONGRID":(gs==OFFGRID)?"OFFGRID":"N/A");
  setLineText(2, 16, buffer);

  int soc_int = (int)soc;           // parte intera
  snprintf(tmp, sizeof(tmp), "%d", soc_int);
  snprintf(buffer, sizeof(buffer), "Temp: %s SOC: %s%% ",
         (ts == TEMP_NORMAL) ? "NORMAL" :
         (ts == TEMP_WARNING) ? "WARNING" :
         (ts == TEMP_CRITICAL) ? "CRITICAL" : "N/A",
         tmp);
  setLineText(3, 24, buffer);
  // Disegna con lo scroll
  updateScrolling();
}