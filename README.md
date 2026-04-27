![osd-demo](osd-demo.GIF)

## Disclaimer

It was (partially) vibe-codded!

## About

Myosd is a simple C++ program made for showing OSD for brightness and volume on wayland compositors. It ins't perfect and was only tested with my personal laptop (arch/hyprland).

## Installation

### AUR
      paru -S myosd-git

### Build from source

Make sure you have the following dependencies installed:
* `gtkmm3`
* `gtk-layer-shell`
* `pamixer` (for volume control)
* `brightnessctl` (for brightness control)

1. Clone the repository:
   ```bash
   git clone https://github.com/moretto08/my-osd.git
   cd my-osd
   ```

2. Run the installer:

    ```bash
    chmod +x install.sh
    ./install.sh
    ```

### Usage

   Add/edit the following to your hyprland.conf:

      #Laptop keys for volume and LCD brightness
      bindel = ,XF86AudioRaiseVolume, exec, /usr/lib/myosd/volume_osd.sh up
      bindel = ,XF86AudioLowerVolume, exec, /usr/lib/myosd/volume_osd.sh down
      bindel = ,XF86AudioMute, exec, /usr/lib/myosd/volume_osd.sh mute
      bindel = ,XF86MonBrightnessUp, exec, /usr/lib/myosd/brightness_osd.sh +
      bindel = ,XF86MonBrightnessDown, exec, /usr/lib/myosd/brightness_osd.sh -
     
      #CAPS and NUM lock
      bindl = , Caps_Lock, exec, /usr/lib/myosd/lock_osd.sh capslock
      bindl = , Num_Lock, exec, /usr/lib/myosd/lock_osd.sh numlock   
 
### Customization & tips

After instalation you can copy the style.css for your config folder using:

      sudo cp /usr/share/myosd/style.css ~/.config/myosd/style.css


By default, css catches wal colors on ~/.cache/wal/colors-gtk.css
if you don't use wal collors, edit the css for desired colors.

To get blur working on hyprland you may need to set 

      layerrule = blur on, ignore_alpha 0.2, match:namespace my-osd
      
on your hyprland.conf
