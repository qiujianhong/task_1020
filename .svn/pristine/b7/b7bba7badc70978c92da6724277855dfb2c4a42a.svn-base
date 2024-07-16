CURDIR= $(shell pwd)
TOPDIR= $(CURDIR)
LIB_TARGET = $(TOPDIR)/target/lib/hal_lib
BIN_TARGET = $(TOPDIR)/target/bin


ARM=1
LIB_TARGET = $(TOPDIR)/target/lib/hal_lib	
CC= /opt/gcc-linaro-4.9-2016.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc -g -rdynamic -funwind-tables -ffunction-sections
CFLAGS= -c -I$(TOPDIR)/lib/include -Wall -O2 -fPIC

LDFLAGS= -L$(TOPDIR) -linterface

export TOPDIR ARM CC CFLAGS LDFLAGS LIB_TARGET BIN_TARGET

DLIB := -lhd
DLIB += -lhal_uart
DLIB += -lhal_led
DLIB += -lhal_key
DLIB += -lhal_lcd
DLIB += -lhal_eeprom
DLIB += -lhal_audio
DLIB += -lhal_watchdog
DLIB += -lhal_esam
DLIB += -lhal_rtc
DLIB += -lhal_power
DLIB += -lhal_acs
DLIB += -lhal_adc
DLIB += -lhal_misc
DLIB += -lhal_bluetooth
DLIB += -lhal_devinfo
export DLIB

EXE= hd adc dataflash acs eeprom rtc misc power esam watchdog audio lcd key led uart bluetooth devinfo

.PHONY := all
all : $(EXE)

hd:
	cd lib/hd && $(MAKE)
adc:
	cd lib/adc && $(MAKE)
dataflash:
	cd lib/dataflash && $(MAKE)
acs:
	cd lib/acs && $(MAKE)
eeprom:
	cd lib/eeprom && $(MAKE)
rtc:
	cd lib/rtc && $(MAKE)
misc:
	cd lib/misc && $(MAKE)
power:
	cd lib/power && $(MAKE)
esam:
	cd lib/esam && $(MAKE)
watchdog:
	cd lib/watchdog && $(MAKE)
audio:
	cd lib/audio && $(MAKE)
lcd:
	cd lib/lcd && $(MAKE)
key:
	cd lib/key && $(MAKE)
led:
	cd lib/led && $(MAKE)
uart:
	cd lib/uart && $(MAKE)
bluetooth:
	cd lib/bluetooth && $(MAKE)
devinfo:
	cd lib/devinfo && $(MAKE)
#test
mktest:
	cd hal_test && $(MAKE)
mktestclean:
	cd hal_test && $(MAKE) clean

clean:
	cd lib/hd && $(MAKE) clean
	cd lib/adc && $(MAKE) clean
	cd lib/dataflash && $(MAKE) clean
	cd lib/acs && $(MAKE) clean
	cd lib/eeprom && $(MAKE) clean
	cd lib/rtc && $(MAKE) clean
	cd lib/misc && $(MAKE) clean
	cd lib/power && $(MAKE) clean
	cd lib/esam && $(MAKE) clean
	cd lib/watchdog && $(MAKE) clean
	cd lib/audio && $(MAKE) clean
	cd lib/lcd && $(MAKE) clean
	cd lib/key && $(MAKE) clean
	cd lib/led && $(MAKE) clean
	cd lib/uart && $(MAKE) clean
	cd lib/bluetooth && $(MAKE) clean
	cd lib/devinfo && $(MAKE) clean
	cd hal_test && $(MAKE) clean

