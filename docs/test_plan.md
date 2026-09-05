# 🧪 Piano di Test di Integrazione PiShield BMS

Questo documento illustra i piani di test per validare la logica del BMS **PiShield BMS**.
Imposta `#define TEST_MODE true` in `config.h` per abilitare i comandi seriali.

---

<details open>
<summary><b>🌩️ 1. Blackout con batteria debole (Warning)</b></summary>

* **Input Seriale:** `SIM_GRID 0` poi `SIM_BATT 3.1`
* **Stato Simulato:** Manca la rete (Offgrid) e la batteria è scesa sotto la soglia di warning (3.2V).
* **Risultato Atteso:** 
  > Il sistema entra in stato `BAT LOW` e `OFFGRID`.
  > Stampa **`SHUTDOWN_REQ`** e inizia un conto alla rovescia (60s) avvisando `SHUTDOWN IMMINENT`. 
  > Il relè rimane *chiuso*.

</details>

<details>
<summary><b>🔌 2. Ripristino Rete durante il Countdown</b></summary>

* **Input Seriale:** `SIM_GRID 5.0` *(durante il test 1)*
* **Stato Simulato:** La corrente elettrica torna prima dello scadere del timer di sicurezza.
* **Risultato Atteso:** 
  > Il sistema riconosce `ONGRID`. 
  > Il timer di spegnimento viene **interrotto e annullato**. 
  > Il sistema riprende il normale funzionamento, il relè non stacca.

</details>

<details>
<summary><b>🤝 3. Acknowledge (Spegnimento RPi)</b></summary>

* **Input Seriale:** `SIM_GRID 0` ➔ `SIM_BATT 3.1` ➔ `ACK`
* **Stato Simulato:** Blackout e batteria debole. Il Raspberry Pi risponde con "ACK" per confermare lo spegnimento imminente.
* **Risultato Atteso:** 
  > Il sistema riceve `ACK` e stampa **`WAITING`**. 
  > Il cooldown viene forzatamente **ridotto a 30s** per velocizzare lo stacco di sicurezza.

</details>

<details>
<summary><b>💀 4. Batteria Critica (Hard Cutoff)</b></summary>

* **Input Seriale:** `SIM_GRID 0` poi `SIM_BATT 2.9`
* **Stato Simulato:** La batteria scende sotto la soglia letale (3.0V). Rischio di danneggiamento chimico.
* **Risultato Atteso:** 
  > Nessun conto alla rovescia. 
  > Stampa **`!!! EMERGENCY CUTOFF TRIGGERED !!!`** con reason **`BATTERY_CRITICAL_AND_OFFGRID`** e **stacca il relè istantaneamente** per salvare la batteria.

</details>

<details>
<summary><b>🔥 5. Surriscaldamento (Warning)</b></summary>

* **Input Seriale:** `SIM_TEMP 71`
* **Stato Simulato:** L'NTC rileva una temperatura superiore ai 70°C.
* **Risultato Atteso:** 
  > Il sistema entra in `TEMP WARNING`. 
  > Stampa **`SHUTDOWN_REQ`** e avvia il cooldown per far spegnere dolcemente il RPi.

</details>

<details>
<summary><b>🌋 6. Surriscaldamento Critico (Thermal Cutoff)</b></summary>

* **Input Seriale:** `SIM_TEMP 80`
* **Stato Simulato:** Temperatura oltre i 75°C. Rischio di esplosione o incendio.
* **Risultato Atteso:** 
  > Stampa **`!!! EMERGENCY CUTOFF TRIGGERED !!!`** con reason **`TEMPERATURE_CRITICAL`** e **stacca il relè istantaneamente**.

</details>

<details>
<summary><b>🛑 7. Spegnimento Manuale Diretto</b></summary>

* **Input Seriale:** `SHUTDOWN_NOW`
* **Stato Simulato:** Utente forza lo spegnimento immediato per manutenzione.
* **Risultato Atteso:** 
  > Stampa **`SHUTDOWN REASON: user_choice`**. 
  > Inizia a stampare **`SHUTDOWN IMMINENT - COOLDOWN: ...`** ogni secondo in modo fluido.
  > Attende 15 secondi esatti e poi disattiva il relè. Non è interrompibile se torna la rete.

</details>

<details>
<summary><b>✅ 8. Ripristino da Sicurezza</b></summary>

* **Input Seriale:** `SIM_RESET` (oppure `SIM_GRID 5.0` e `SIM_TEMP 25`)
* **Stato Simulato:** Rete elettrica ripristinata e temperature tornate normali.
* **Risultato Atteso:** 
  > Stampa **`[RECOVERY] Rete ripristinata e temperatura sicura. Riattacco il rele'.`**.
  > I log tornano a indicare `ONGRID` e `T:OK`. Il BMS rientra a regime normale ricaricando la batteria.

</details>
