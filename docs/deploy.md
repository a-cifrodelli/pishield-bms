# 🚀 Guida al Deployment e Installazione Software (PiShield BMS)

Questa guida descrive la procedura di installazione e configurazione del software di integrazione su **Raspberry Pi** (o altro Single Board Computer Linux) per la gestione della comunicazione seriale, il log degli eventi e l'arresto controllato del sistema operativo (*graceful shutdown*).

---

## 📋 Requisiti Preliminari

1. **Sistema Operativo**: Raspberry Pi OS (o altra distribuzione Linux basata su Debian).
2. **Connessione Seriale**:
   * Collegamento UART tra Arduino Nano (tramite traslatore di livello logico 3.3V-5V) ed il Raspberry Pi (su porta `/dev/ttyUSB0` o GPIO UART `/dev/ttyAMA0` / `/dev/ttyS0`).
3. **Abilitazione UART su Raspberry Pi**:
   * Eseguire `sudo raspi-config` -> `Interface Options` -> `Serial Port`.
   * Disabilitare la console di login sulla seriale (*Would you like a login shell to be accessible over serial?* -> **No**).
   * Abilitare l'hardware della porta seriale (*Would you like the serial port hardware to be enabled?* -> **Yes**).
   * In alternativa, verificare che in `/boot/firmware/config.txt` (o `/boot/config.txt`) sia presente la riga:
     ```ini
     enable_uart=1
     ```

---

## ⚡ Installazione Automatica via Script (`install.sh`)

È fornito uno script bash automatizzato in `integration/install.sh` che gestisce la creazione dell'ambiente virtuale Python (`venv`), l'installazione delle dipendenze da `requirements.txt`, la copia dei file in `/opt/pishield_bms`, la configurazione dei permessi e l'attivazione del servizio Systemd `pishield-bms.service`.

### Procedura di Installazione

1. Clonare il repository sul Raspberry Pi:
   ```bash
   git clone https://github.com/username/pishield_bms.git
   cd pishield_bms/integration
   ```

2. Rendere eseguibile ed avviare lo script di installazione con i privilegi di root:
   ```bash
   chmod +x install.sh launcher.sh
   sudo ./install.sh
   ```

---

## ⚙️ Operazioni Eseguite dall'Installer

Lo script di installazione esegue automaticamente i seguenti passaggi:

1. **Installazione Pacchetti di Sistema**:
   * Installa `python3`, `python3-venv` e `python3-pip`.
2. **Setup della Directory di Servizio (`/opt/pishield_bms`)**:
   * Copia i file del modulo: `bms_integration.py`, `config.py`, `requirements.txt`, `launcher.sh` e crea un file `.env` basato su `.env.example`.
3. **Ambiente Virtuale Python (Venv)**:
   * Inizializza l'ambiente virtuale in `/opt/pishield_bms/venv` ed installa le dipendenze isolate definite in `requirements.txt` (`pyserial`, `paho-mqtt`, `python-dotenv`).
4. **Configurazione Symlink Globale**:
   * Crea i link simbolici `/usr/local/bin/pishield-bms` ed `/usr/local/bin/bms_integration` -> `/opt/pishield_bms/launcher.sh` per consentire l'esecuzione da qualsiasi terminale.
5. **Configurazione Permessi Sudoers**:
   * Crea il file `/etc/sudoers.d/pishield-bms` per consentire l'esecuzione del comando `/sbin/shutdown` senza richiesta di password:
     ```text
     ALL ALL=(ALL) NOPASSWD: /sbin/shutdown
     ```
6. **Attivazione Servizio Systemd**:
   * Installa ed abilita `/etc/systemd/system/pishield-bms.service` avviando in background `/opt/pishield_bms/launcher.sh ack`.

---

## 🛠️ Personalizzazione tramite File `.env`

Le impostazioni del client (porta seriale, baudrate, file di log e parametri MQTT) sono centralizzate in `config.py` e possono essere personalizzate modificando il file `/opt/pishield_bms/.env`:

```env
SERIAL_PORT=/dev/ttyUSB0
BAUDRATE=115200
LOGFILE=/var/log/pishield_bms.log
SUFFIX_FILTER=LOG

MQTT_HOST=localhost
MQTT_PORT=8883
MQTT_USER=
MQTT_PASSWORD=
MQTT_TLS=false
MQTT_CA_CERTS=
```

---

## 🔍 Verifiche e Utilizzo CLI

### Verificare lo Stato del Servizio
Per controllare che il servizio in background stia ascoltando correttamente i comandi seriali dall'Arduino:

```bash
systemctl status pishield-bms.service
```

Per consultare i log del servizio tramite `journalctl`:
```bash
journalctl -u pishield-bms.service -f
```

### Utilizzo dei Comandi CLI Manuali
Il comando `pishield-bms` invoca lo script `launcher.sh` eseguendo la logica nel venv dedicato:

* **Test Connessione (PING)**:
  ```bash
  pishield-bms ping
  ```
* **Ascolto e Log Seriale**:
  ```bash
  pishield-bms listen
  ```
* **Lettura Log Salvati**:
  ```bash
  pishield-bms logs -f
  ```
* **Forzatura Spegnimento Manuale**:
  ```bash
  pishield-bms shutdown
  ```
