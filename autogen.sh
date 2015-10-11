#!/bin/bash
# autogen.sh
#----------------------------------------------------------
# Copyright(C) 2015 Lorenzo Delana, License under MIT
#
#   https://github.com/devel0/ArduiPi_SSD1306
#
# Summary : adapt code to be bananapi compatible, in short
#           - set i2c-2 device port
#           - adjust CCFLAGS for the library and examples

BD=`dirname $0`
BASEDIR=`realpath $BD`

echo "Specify your platform:"
echo "  1. RaspberryPI"
echo "  2. BananaPI"

read -n 1 c
echo

if [ "$c" == "1" ]; then
	echo "Setting for RaspberryPI"
	HW="RaspberryPI"
elif [ "$c" == "2" ]; then
	echo "Setting for BananaPI"
	HW="BananaPI"
else
	echo "Invalid argument given."
	HW="RaspberryPI" # fallback to raspi
	exit
fi

echo "Ensure libi2c-dev packages"
apt-get install libi2c-dev

echo $HW > $BASEDIR/hwplatform

echo "run make to build"

