#
# Makefile for a linux user module and wiringPi
#
# Copyright 2020 Felix Knorre  <felix-knorre@hotmail.de>
#

readMod : getTime.c
	gcc -Wall -o getTime getTime.c -lwiringPi -lwiringPiDev
	
clean : 
	rm getTime
