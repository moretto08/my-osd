#!/bin/bash

case $1 in
    up)
        pamixer -i 5 --set-limit 100
        ;;
    down)
        pamixer -d 5
        ;;
    mute)
        pamixer -t
        ;;
esac

pkill -9 myosd 2>/dev/null

VOL=$(pamixer --get-volume)
IS_MUTED=$(pamixer --get-mute)

if [ "$IS_MUTED" = "true" ]; then
    ICON="audio-volume-muted-symbolic"
elif [ "$VOL" -le 30 ]; then
    ICON="audio-volume-low-symbolic"
elif [ "$VOL" -le 70 ]; then
    ICON="audio-volume-medium-symbolic"
else
    ICON="audio-volume-high-symbolic"
fi

~/Documentos/prog/C_C++/my-osd/build/myosd "$VOL" "$ICON" &
