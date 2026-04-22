
#!/bin/bash
brightnessctl set 5%${1} # $1 -> + or -
VOL=$(brightnessctl i | grep -oP '\(\K\d+(?=%\))')
/usr/local/bin/myosd "$VOL" "display-brightness-symbolic" &
