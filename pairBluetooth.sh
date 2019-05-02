#!/bin/bash

NAME="OBDII"

if [ $# = 1 ]; then
	NAME="$1"
fi
echo "$NAME"

MAC_BT=$(hcitool scan | grep "$NAME" | awk '{ print $1 }')

echo "$MAC_BT"

echo -e "power on\nagent on\nscan on\ntrust $MAC_BT\nconnect $MAC_BT \nquit" | bluetoothctl
