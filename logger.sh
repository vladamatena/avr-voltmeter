#!/bin/sh

mv data data.`date +%s`

while true; do

	stty -F /dev/ttyUSB0 raw speed 9600 -crtscts cs8 -parenb -cstopb
	cat /dev/ttyUSB0 | tee -a data

done;
