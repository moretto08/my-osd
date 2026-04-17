
#!/bin/bash
brightnessctl set 5%${1} # $1 -> + or -
pkill -9 myosd
VOL=$(brightnessctl i | grep -oP '\(\K\d+(?=%\))')
$HOME/Documentos/prog/C_C++/my-osd/build/myosd "$VOL" "display-brightness-symbolic" &
