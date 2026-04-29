#!/bin/bash
LED=$1
STATE_FILE="/tmp/myosd_state_${LED}"

if [ ! -f "$STATE_FILE" ]; then
    echo "0" > "$STATE_FILE"
fi

VAL=$(cat "$STATE_FILE")

if [ "$VAL" == "0" ]; then
    NEW_VAL="1"
    STATE="ON"
    ICON="${LED}-enabled"
    OSD_TEXT="Num Lock ON"
else
    NEW_VAL="0"
    STATE="OFF"
    ICON="${LED}-disabled"
    OSD_TEXT="Num Lock OFF"
fi

echo "$NEW_VAL" > "$STATE_FILE"

/usr/bin/myosd "-1" "$ICON" "$OSD_TEXT" &
