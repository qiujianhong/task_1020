SRC:= $(wildcard *.c)
#TGT:= $(SRC:.c=.o)
TGT += libhal_pulse.so

.PHONY := all
all : $(TGT)

%.o : %.c
	$(CC) $(CFLAGS) $?

libhal_pulse.so : *.o
	$(CC) -shared -o $@ *.o
	rm -f *.o
	cp *.so $(LIB_TARGET)
	
clean :
	rm -f $(TGT) *~	
        

 
