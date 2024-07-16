SRC := $(wildcard *.c)
TGT := $(patsubst %.c,%,$(SRC))
HAL_INCLUDE = ../../../hal_include
all : $(TGT)

% : %.c
	$(CC) -I$(HAL_INCLUDE) -Wall -O2 -lhd -ldl -L$(LIB_TARGET) -pthread $? -o $(BIN_TARGET)/$@ -Wl,-rpath=/lib/hal_lib

clean:
	rm -f $(TGT)

