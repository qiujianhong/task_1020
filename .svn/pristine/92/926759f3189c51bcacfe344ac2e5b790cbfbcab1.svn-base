SRC := $(wildcard *.c)
TGT := $(patsubst %.c,%,$(SRC))

all : $(TGT)

% : %.c
	$(CC) -I$(TOPDIR)/lib/include -Wall -O2 -lhd -ldl -L$(LIB_TARGET) -pthread $? -o $(BIN_TARGET)/$@ -Wl,-rpath=/mnt/hal_lib

clean:
	rm -f $(TGT)

