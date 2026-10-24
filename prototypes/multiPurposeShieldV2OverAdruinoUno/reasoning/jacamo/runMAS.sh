#! /bin/bash
JACAMO="/usr/bin/jacamo"
clear

if [[ ! -f "$JACAMO" ]]
then
    echo "The computer hasn't all required dependencies!"
    echo "Installing dependencies..."
    sleep 3
    echo "deb [trusted=yes] http://packages.chon.group/ chonos main" | sudo tee /etc/apt/sources.list.d/chonos.list
    sudo apt update
    sudo apt install -y jacamo-cli
else
    echo "The computer has all required dependencies"
fi

echo "Starting the MAS!"
jacamo shieldNineInOne.mas2j
echo "MAS Stopped!"
