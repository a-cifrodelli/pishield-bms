#!/usr/bin/env bash
# ==============================================================================
# PiShield BMS - Virtual Environment Launcher Script
# ==============================================================================
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENV_DIR="${SCRIPT_DIR}/venv"

# 1. Creazione del virtual environment se non esiste
if [ ! -d "${VENV_DIR}" ]; then
  echo "[VENV] Creazione virtual environment Python in: ${VENV_DIR}"
  python3 -m venv "${VENV_DIR}"
fi

# 2. Attivazione dell'ambiente virtuale
source "${VENV_DIR}/bin/activate"

# 3. Verifiche ed installazione delle dipendenze da requirements.txt
if [ -f "${SCRIPT_DIR}/requirements.txt" ]; then
  pip install --quiet --disable-pip-version-check -r "${SCRIPT_DIR}/requirements.txt"
fi

# 4. Esecuzione dello script bms_integration.py nel contesto del venv
exec python3 "${SCRIPT_DIR}/bms_integration.py" "$@"
