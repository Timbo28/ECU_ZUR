#!/bin/bash

# Anzahl der gesendeten Nachrichten initialisieren
count=0

# Startzeit in Sekunden seit Epoche
start_time=$(date +%s)
end_time=$((start_time + 60))

# Temporäre Datei erstellen, um die Ausgabe von cangen zu speichern
tempfile=$(mktemp)

# cangen im Hintergrund starten und seine Ausgabe in die temporäre Datei umleiten
cangen can1 -g 0  -I 1 -L 8 -p 10  &


# PID des cangen-Prozesses speichern
cangen1_pid=$!

#cangen can1 -g 0  -I 2 -L 8 -p 10  &
#cangen2_pid=$!

# Schleife, die 60 Sekunden lang läuft
while [ $(date +%s) -lt $end_time ]; do

    sleep 0.001  # Kurze Pause, um die CPU-Last zu reduzieren
done

# cangen-Prozess beenden
kill $cangen1_pid
#kill $cangen2_pid



# Anzahl der gesendeten Nachrichten ausgeben
echo "Anzahl der gesendeten Nachrichten"


