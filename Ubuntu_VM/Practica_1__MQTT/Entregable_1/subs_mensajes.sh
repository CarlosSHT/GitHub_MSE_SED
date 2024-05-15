##!/bin/bash
# Configuración del broker y topic a escuchar.
# Configuración inicial:
broker="localhost"
port="1883"
topic="practica1/rssi"
archivo="pc1_recibidos.txt"
#--------------------------------------------------
# Ponemos el cliente de mosquitto a escuchar
mosquitto_sub -t $topic -h $broker -p $port -v | while read value;    do
ts=$(date "+%d/%m/%Y %H:%M:%S")             # Agregamos una marca de tiempo en la variable ts (opcional).

rssi=$(echo "$value" | cut -d' ' -f2)       # Obtiene el valor RSSI
msg="$ts RSSI: $rssi [dBm]"

# Guardamos valores:
echo "$msg" >> $archivo    # guardamos datos en archivo
echo "$msg"                # mostramos el resultado por consola
done
