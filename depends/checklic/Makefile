CURDIR=$(shell pwd)

HAL_INCLUDE=../../drivers/hal_include
LIB_TARGET=../../release/hal_lib
CC=/opt/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc

all :
	$(CC) -I$(HAL_INCLUDE) -Wall -O2 -fPIC *.c -o checklic lhd -ldl -L$(LIB_TARGET) -Wl,-rpath=/lib/hal_lib
	cp checklic $(CURDIR)/../../release/package/
clean:
	rm checklic
