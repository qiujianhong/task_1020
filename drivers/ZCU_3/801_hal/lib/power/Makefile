SRC:= $(wildcard *.c)
#TGT:= $(SRC:.c=.o)
TGT += libhal_power.so

.PHONY := all
all : $(TGT)

%.o : %.c
	$(CC) $(CFLAGS) $?

libhal_power.so : *.o
	$(CC) -shared -o $@ *.o
	rm -f *.o
	cp *.so $(LIB_TARGET)
	
clean :
	rm -f $(TGT) *~	
        

 
