SRC := $(wildcard *.c)
TGT := $(patsubst %.c,%,$(SRC))

all : $(TGT)

% : %.c
	$(CC) -I$(TOPDIR)/../../hal_include -I$(TOPDIR)/lib/include -Wall -O2 -lhd -ldl -L$(LIB_TARGET) -pthread $? -o $(BIN_TARGET)/$@ -Wl,-rpath=/lib/hal_lib

clean:
	rm -f $(TGT)

