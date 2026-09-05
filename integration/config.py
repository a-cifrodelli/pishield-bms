import os
from pathlib import Path
from dotenv import load_dotenv

# Cerca prima nella directory corrente dello script, poi in /etc/pishield_bms/.env o /etc/bms_integration/.env
BASE_DIR = Path(__file__).resolve().parent
ENV_PATH = BASE_DIR / ".env"
SYSTEM_ENV_PATH = Path("/etc/pishield_bms/.env")
LEGACY_SYSTEM_ENV_PATH = Path("/etc/bms_integration/.env")

if ENV_PATH.exists():
    load_dotenv(dotenv_path=ENV_PATH)
elif SYSTEM_ENV_PATH.exists():
    load_dotenv(dotenv_path=SYSTEM_ENV_PATH)
elif LEGACY_SYSTEM_ENV_PATH.exists():
    load_dotenv(dotenv_path=LEGACY_SYSTEM_ENV_PATH)
else:
    load_dotenv()  # Fallback a variabili d'ambiente di sistema

# ==============================================================================
# CONFIGURAZIONE SERIALE & LOG
# ==============================================================================
SERIAL_PORT = os.getenv("SERIAL_PORT", "/dev/ttyUSB0")
BAUDRATE = int(os.getenv("BAUDRATE", "115200"))
LOGFILE = os.getenv("LOGFILE", "/var/log/pishield_bms.log")
SUFFIX_FILTER = os.getenv("SUFFIX_FILTER", "LOG")

# ==============================================================================
# CONFIGURAZIONE MQTT
# ==============================================================================
MQTT_HOST = os.getenv("MQTT_HOST", "localhost")
MQTT_PORT = int(os.getenv("MQTT_PORT", "8883"))
MQTT_USER = os.getenv("MQTT_USER", None) or None
MQTT_PASSWORD = os.getenv("MQTT_PASSWORD", None) or None
MQTT_TLS = os.getenv("MQTT_TLS", "false").lower() in ("true", "1", "yes")
MQTT_CA_CERTS = os.getenv("MQTT_CA_CERTS", None) or None
