#
# Makefile for a linux kernel module
#
# Copyright 2020 Felix Knorre  <felix-knorre@hotmail.de>
#

ifneq ($(KERNELRELEASE),) 
obj-m	:= rtcPi.o
# my_kernel_mod-objs  := my_km.o reg.o


else
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

default:
	$(MAKE)	-C $(KDIR)	M=$(PWD) modules

clean: 
	$(MAKE)	-C $(KDIR)	M=$(PWD) clean

endif
