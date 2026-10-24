#! /bin/bash
JASON="/usr/bin/jason"
clear

if [[ ! -f "$JASON" ]]
then
    echo "The computer hasn't all required dependencies!"
    echo "Installing dependencies..."
    sleep 3
    echo "deb [trusted=yes] http://packages.chon.group/ chonos main" | sudo tee /etc/apt/sources.list.d/chonos.list
    sudo apt update
    sudo apt install -y jason-cli
else
    echo "The computer has all required dependencies"
fi

echo "Starting the MAS!"
jason shieldNineInOne.mas2j
echo "MAS Stopped!"
