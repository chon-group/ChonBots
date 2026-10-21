#! /bin/bash
JASONEMBEDDED="/usr/bin/jasonEmbedded"
HERE="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $HERE
clear

if [[ ! -f "$JASONEMBEDDED" ]]
then
    echo "The computer hasn't all required dependencies!"
    echo "Installing dependencies..."
    sleep 3
    echo "deb [trusted=yes] http://packages.chon.group/ chonos main" | sudo tee /etc/apt/sources.list.d/chonos.list
    sudo apt update
    sudo apt install -y jason-embedded
else
    echo "The computer has all required dependencies"
fi

echo "Starting the MAS!"
jasonEmbedded shieldNineInOne.mas2j
echo "MAS Stopped!"
