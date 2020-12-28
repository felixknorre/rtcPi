#!/bin/bash

sudo insmod rtcI2C.ko
sudo insmod rtcPi.ko

echo "rtcPi Driver loaded!"
