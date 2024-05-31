##!/bin/bash
# Configuración del broker y topic a escuchar.
# Configuración inicial:
broker="localhost"
port="1883"
topic="practica2/LDR_data"
archivo="pc2_recibidos.csv"

if [ -f "$archivo" ]; then
    rm "$archivo"
fi
    
echo "Muestreo,Valor" > "$archivo"

mosquitto_sub -t $topic -h $broker -p $port -v | while read -r  message;    do

payload=$(echo "$message" | cut -d ' ' -f 2-)


miliseconds=$(echo "$payload" | jq '.miliseconds')
val_ldr=$(echo "$payload" | jq '.val_ldr')

echo "$miliseconds,$val_ldr"
echo "$miliseconds,$val_ldr" >> $archivo 
done
