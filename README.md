## Disclaimer:

It was vibe-codded!

## About

My-osd is a simple C++ program made for showing OSD for brightness and volume on wayland compositors. It ins't perfect and was only tested with my personal laptop (arch/hyprland).

## Installation

### Prerequisites
Make sure you have the following dependencies installed:
* `gtkmm3`
* `gtk-layer-shell`
* `pamixer` (for volume control)
* `brightnessctl` (for brightness control)

### Quick Start
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

Add the following to your hyprland.conf:

    bindel = , XF86AudioRaiseVolume, exec, $HOME/.config/myosd/volume_osd.sh up
    bindel = , XF86AudioLowerVolume, exec, $HOME/.config/myosd/volume_osd.sh down
    bindel = , XF86MonBrightnessUp, exec, $HOME/.config/myosd/brightness_osd.sh +
    bindel = , XF86MonBrightnessDown, exec, $HOME/.config/myosd/brightness_osd.sh -

    #CAPS and NUM lock
    bindl = , Caps_Lock, exec, $HOME/.config/my-osd/lock_osd.sh capslock
    bindl = , Num_Lock, exec, $HOME/.config/my-osd/lock_osd.sh numlock
    
### Customization

The OSD appearance can be fully customized by editing the CSS file located at:
   /usr/local/share/my-osd/style.css

