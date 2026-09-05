#!/usr/bin/env python3
import argparse
import serial
import sys
import time
import os
import threading
import atexit
import paho.mqtt.client as mqtt

import config

SERIAL_CONN = None

def publish_mqtt(topic: str, message: str,
                 host=config.MQTT_HOST, port=config.MQTT_PORT,
                 user=config.MQTT_USER, password=config.MQTT_PASSWORD,
                 tls_enabled=config.MQTT_TLS, tls_ca=config.MQTT_CA_CERTS,
                 keepalive=60):
    try:
        client = mqtt.Client()
        if user and password:
            client.username_pw_set(user, password)
        if tls_enabled:
            if tls_ca:
                client.tls_set(ca_certs=tls_ca)
            else:
                client.tls_set()  # usa certificati default
        client.connect(host, port, keepalive)
        client.publish(topic, message)
        client.disconnect()
        print(f"MQTT pubblicato → {topic}: {message}")
    except Exception as e:
        print(f"Errore MQTT: {e}")

def open_serial():
    global SERIAL_CONN
    SERIAL_CONN = serial.Serial(config.SERIAL_PORT, config.BAUDRATE, timeout=1)
    return SERIAL_CONN

def cleanup():
    if SERIAL_CONN and SERIAL_CONN.is_open:
        SERIAL_CONN.close()
        print("Seriale chiusa correttamente.")

def write_log(line: str):
    """Scrive log filtrato su file"""
    if line.endswith(config.SUFFIX_FILTER):
        return
    try:
        with open(config.LOGFILE, "a") as f:
            f.write(line.strip() + "\n")
    except Exception as e:
        print(f"Errore scrittura log su {config.LOGFILE}: {e}")

def read_serial(ser):
    """Legge una riga dalla seriale"""
    try:
        line = ser.readline().decode(errors="ignore").strip()
        if line:
            print(line)       # output sempre completo a schermo
            write_log(line)   # log filtrato
        return line
    except Exception:
        return ""

# ---------------------------
# COMMANDS
# ---------------------------
def cmd_shutdown():
    ser = open_serial()
    ser.write(b"SHUTDOWN_NOW\n")
    print("Inviato: SHUTDOWN_NOW")

def cmd_ping():
    ser = open_serial()
    ser.write(b"PING\n")
    print("Inviato: PING")
    time.sleep(0.5)
    read_serial(ser)

def ack_listener():
    """Thread che risponde con ACK quando riceve SHUTDOWN_REQ"""
    ser = open_serial()
    print("ACK listener attivo... (CTRL+C per uscire)")
    try:
        while True:
            line = read_serial(ser)
            if line == "SHUTDOWN_REQ":
                ser.write(b"ACK\n")
                print("Inviato: ACK")
    except KeyboardInterrupt:
        print("\nACK listener interrotto.")

def cmd_ack():
    ser = open_serial()
    print("Listener ACK avviato, in attesa di SHUTDOWN_REQ...")
    while True:
        line = ser.readline().decode(errors="ignore").strip()
        if not line:
            continue
        print("Ricevuto:", line)

        if line == "SHUTDOWN_REQ":
            ser.write(b"ACK\n")
            print("Inviato: ACK")

            # Pubblica su MQTT
            publish_mqtt("system/shutdown", "ACK sent, shutting down")

            # Spegni Raspberry
            os.system("sudo shutdown -h now")
            break

def cmd_listen():
    """Resta in ascolto e gestisce seriale + log"""
    ser = open_serial()
    print("In ascolto sulla seriale... (CTRL+C per uscire)")
    try:
        while True:
            read_serial(ser)
    except KeyboardInterrupt:
        print("\nInterrotto.")

SKIP_THRESHOLD = 3  # salva solo 1 entry ogni 3 righe

def cmd_logs(follow=False):
    """Mostra log salvati (già filtrati) con skip threshold globale"""
    if not os.path.exists(config.LOGFILE):
        print(f"Nessun log presente in {config.LOGFILE}.")
        return

    counter = 0

    # stampa log attuale filtrando
    with open(config.LOGFILE, "r") as f:
        for line in f:
            line = line.strip()
            if counter % SKIP_THRESHOLD == 0:
                print(line)
            counter += 1

    if follow:
        print("\n--- Seguendo i log (CTRL+C per uscire) ---\n")
        with open(config.LOGFILE, "r") as f:
            f.seek(0, os.SEEK_END)
            counter = 0
            try:
                while True:
                    line = f.readline()
                    if not line:
                        time.sleep(0.5)
                        continue
                    line = line.strip()
                    if counter % SKIP_THRESHOLD == 0:
                        print(line)
                    counter += 1
            except KeyboardInterrupt:
                print("\nUscita da live logs.")


# ---------------------------
# MAIN ENTRYPOINT
# ---------------------------
def main():
    parser = argparse.ArgumentParser(description="PiShield BMS - Client seriale e daemon di integrazione per Raspberry Pi")
    sub = parser.add_subparsers(dest="cmd")

    sub.add_parser("shutdown", help="Invia SHUTDOWN_NOW")
    sub.add_parser("ack", help="Resta in attesa di SHUTDOWN_REQ e risponde ACK")
    sub.add_parser("ping", help="Invia PING e mostra risposta")
    sub.add_parser("listen", help="Ascolta continuamente la seriale e logga (filtrato)")
    logs_parser = sub.add_parser("logs", help="Mostra log salvati")
    logs_parser.add_argument("-f", action="store_true", help="Segui log live")

    args = parser.parse_args()

    if args.cmd == "shutdown":
        cmd_shutdown()
    elif args.cmd == "ack":
        cmd_ack()
    elif args.cmd == "ping":
        cmd_ping()
    elif args.cmd == "listen":
        cmd_listen()
    elif args.cmd == "logs":
        cmd_logs(follow=args.f)
    else:
        parser.print_help()

if __name__ == "__main__":
    atexit.register(cleanup)
    main()
