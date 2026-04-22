#!/bin/bash
LED=$1
# Arquivo para salvar o estado (0 para OFF, 1 para ON)
STATE_FILE="/tmp/myosd_state_${LED}"

# Se o arquivo não existe, cria como 0 (OFF)
if [ ! -f "$STATE_FILE" ]; then
    echo "0" > "$STATE_FILE"
fi

# Lê o estado atual
VAL=$(cat "$STATE_FILE")

# Alterna o valor
if [ "$VAL" == "0" ]; then
    NEW_VAL="1"
    STATE="ON"
    ICON="${LED}-enabled"
else
    NEW_VAL="0"
    STATE="OFF"
    ICON="${LED}-disabled"
fi

# Salva o novo estado IMEDIATAMENTE
echo "$NEW_VAL" > "$STATE_FILE"

# Envia para o OSD
/usr/local/bin/myosd "-1" "$ICON" "$STATE" &