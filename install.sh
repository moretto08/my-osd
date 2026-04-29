#!/bin/bash

# Terminal Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${BLUE}Starting MyOSD installation...${NC}"


echo -e "${GREEN}Compiling source code...${NC}"
if make; then
    echo "Build successful."
else
    echo -e "${RED}Build failed.${NC} Please check if dependencies (gtkmm-3.0, gtk-layer-shell-0) are installed."
    exit 1
fi


echo -e "${GREEN}Installing binary to system (root password may be required)...${NC}"
if sudo make install; then
    echo "System files installed successfully."
else
    echo -e "${RED}Failed to install system files.${NC}"
    exit 1
fi

echo -e "${BLUE}Installation complete!${NC}"
echo "Don't forget to update your hyprland.conf to point to /usr/lib/myosd/ script."
