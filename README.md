# 🛡️ PiShield BMS - Smart UPS & BMS for Raspberry Pi / SBC

![Project Status: WIP](https://img.shields.io/badge/status-WIP-orange.svg)
![Platform: Arduino](https://img.shields.io/badge/platform-Arduino-00979D.svg)
![Host: Raspberry%20Pi](https://img.shields.io/badge/host-Raspberry%20Pi-C51A4A.svg)
![Python: 3.x](https://img.shields.io/badge/python-3.x-3776AB.svg)
![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)

**PiShield BMS** è un sistema **UPS (Uninterruptible Power Supply)** personalizzato e modulare dotato di **BMS (Battery Management System)** intelligente basato su Arduino Nano, progettato per garantire l'alimentazione continua e lo spegnimento sicuro (*graceful shutdown*) di un Raspberry Pi o altro Single Board Computer Linux.

> [!WARNING]
> **Work In Progress (WIP)**: Il progetto è in fase di affinamento. La custodia / enclosure stampata in 3D per alloggiare l'elettronica è stata prototipata ma non ha ancora raggiunto una maturità progettuale idonea per la pubblicazione (versione definitiva in corso di perfezionamento).

---

## 💡 Motivazione e Filosofia del Progetto

Il progetto nasce dalla necessità di realizzare un UPS con BMS integrato per Raspberry Pi **senza dover ricorrere alle schede commerciali preassemblate in formato HAT**. 

I moduli HAT commerciali presentano spesso limiti quali:
* Scarsa personalizzazione delle soglie di sicurezza e dei tempi di spegnimento.
* Assenza di protezione termica attiva sulle singole celle 18650.
* Impossibilità di effettuare uno sgancio fisico (*hard cutoff*) per preservare le batterie da scarica profonda e degrado chimico.
* Ingombro rigido sovrapposto al Raspberry Pi.

**PiShield BMS** unisce un economico modulo UPS base con circuito di carica/boost (LX-2BUPS) ad una logica di controllo dedicata su **Arduino Nano**, garantendo la massima sicurezza hardware e flessibilità software.

---

## 📚 Indice della Documentazione

Tutta la documentazione di dettaglio è organizzata nella cartella `docs/`:

* 🛠️ **[Guida alla Ricreazione Hardware (BOM, Cablaggio e Saldature)](docs/recreate.md)**: Elenco componenti (BOM), schema elettrico, istruzioni per le saldature custom ed adattamento del modulo mini UPS base.
* 🚀 **[Guida al Deployment e Installazione Software](docs/deploy.md)**: Configurazione della seriale su Raspberry Pi, script di installazione automatica (`install.sh`), configurazione Systemd e comandi CLI (`pishield-bms`).
* 🧪 **[Piano di Test di Integrazione e Simulazione](docs/TEST_PLAN.md)**: Scenari di test simulati (`TEST_MODE`) per validare spegnimenti, blackout, recovery ed emergency cutoff.

---

## ✨ Funzionalità Principali

* **Doppia Protezione Batterie (Hard Cutoff & Graceful Shutdown)**:
  * **Spegnimento Morbido (Graceful Shutdown)**: Invia una richiesta seriale `SHUTDOWN_REQ` al Raspberry Pi al superamento di soglie di warning (tensione batteria `< 3.2V` o temperatura `> 70°C`).
  * **Sgancio di Emergenza (Hard Cutoff)**: Disattiva il relè KY-019 per staccare fisicamente il pacco batterie in condizioni estreme (tensione batteria `< 3.0V` o temperatura `> 75°C`).
* **Stadi di Protezione Ingressi Analogici**:
  * Lettura filtrata delle tensioni di rete e batteria tramite partitori protetti da fusibili autoripristinanti PPTC (`X16 CF110`) e diodi Schottky (`1N5819`).
* **Monitoraggio Termico**:
  * Misurazione della temperatura su 2 zone/celle tramite termistori NTC 100kΩ.
* **Interfaccia OLED SSD1306 (128x32)**:
  * Visualizzazione in tempo reale di tensioni, temperature, stato della rete, stato delle batterie e calcolo del SOC % con testo in scorrimento continuo.
* **Integrazione Host (Raspberry Pi)**:
  * Client Python (`bms_integration.py` con `config.py` e `.env`) gestito da demone Systemd (`pishield-bms.service`) per la gestione automatica degli ACK di spegnimento e pubblicazione dello stato su broker **MQTT**.
* **Ripristino Automatico (Auto-Recovery)**:
  * Riaggancio automatico del relè quando viene ripristinata la rete elettrica principale e le temperature tornano nella norma.

---

## 🔌 Schema Elettrico di Sintesi

![Diagramma di Cablaggio](docs/media/wiring.png)

Per i dettagli completi sul cablaggio e sulle saldature, fare riferimento alla **[Guida alla Ricreazione Hardware](docs/recreate.md)**.

---

## 📄 Licenza

Questo progetto è rilasciato sotto licenza **[MIT License](LICENSE)**.

