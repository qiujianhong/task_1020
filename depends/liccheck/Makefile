CURDIR=$(shell pwd)

CC=/opt/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc

all :
	$(CC) -Wall -O2 -fPIC *.c -o liccheck
	cp liccheck $(CURDIR)/../../release/package/
clean:
	rm liccheck
