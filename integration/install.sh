#!/usr/bin/env bash
# ==============================================================================
# PiShield BMS - Host Integration Installer (with Venv support)
# ==============================================================================
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALL_DIR="/opt/pishield_bms"

echo "=== Installazione PiShield BMS Daemon (Venv & Config) ==="

if [ "$EUID" -ne 0 ]; then
  echo "[ERROR] Questo script deve essere eseguito con i privilegi di root (es. sudo ./install.sh)"
  exit 1
fi

echo "[1/6] Installazione pacchetti di sistema di base (python3-venv)..."
apt-get update -qq
apt-get install -y -qq python3 python3-venv python3-pip

echo "[2/6] Preparazione directory di installazione in ${INSTALL_DIR}..."
mkdir -p "${INSTALL_DIR}"

cp "${SCRIPT_DIR}/bms_integration.py" "${INSTALL_DIR}/"
cp "${SCRIPT_DIR}/config.py" "${INSTALL_DIR}/"
cp "${SCRIPT_DIR}/requirements.txt" "${INSTALL_DIR}/"
cp "${SCRIPT_DIR}/launcher.sh" "${INSTALL_DIR}/"
chmod +x "${INSTALL_DIR}/launcher.sh"

if [ ! -f "${INSTALL_DIR}/.env" ]; then
  echo "[+] Creazione file di configurazione .env predefinito..."
  cp "${SCRIPT_DIR}/.env.example" "${INSTALL_DIR}/.env"
fi

echo "[3/6] Inizializzazione Virtual Environment e installazione dipendenze..."
"${INSTALL_DIR}/launcher.sh" ping > /dev/null 2>&1 || true

echo "[4/6] Configurazione Symlink globale in /usr/local/bin/pishield-bms..."
ln -sf "${INSTALL_DIR}/launcher.sh" /usr/local/bin/pishield-bms
ln -sf "${INSTALL_DIR}/launcher.sh" /usr/local/bin/bms_integration

echo "[5/6] Configurazione regole Sudoers per lo spegnimento senza password..."
cat << 'EOF' > /etc/sudoers.d/pishield-bms
ALL ALL=(ALL) NOPASSWD: /sbin/shutdown
EOF
chmod 0440 /etc/sudoers.d/pishield-bms

echo "[6/6] Installazione e attivazione del servizio Systemd (pishield-bms.service)..."
cp "${SCRIPT_DIR}/installation/pishield-bms.service" /etc/systemd/system/pishield-bms.service
chmod 644 /etc/systemd/system/pishield-bms.service

systemctl daemon-reload
systemctl enable pishield-bms.service
systemctl restart pishield-bms.service

echo "=============================================================================="
echo " OK Installazione PiShield BMS completata con successo!"
echo " Directory installazione: ${INSTALL_DIR}"
echo " File di configurazione:  ${INSTALL_DIR}/.env"
echo " Servizio Systemd:        systemctl status pishield-bms.service"
echo " Comandi CLI:             pishield-bms ping | pishield-bms logs -f"
echo "=============================================================================="
