SRC:= $(wildcard *.c)
#TGT:= $(SRC:.c=.o)
TGT += libhal_dataflash.so

.PHONY := all
all : $(TGT)

%.o : %.c
	$(CC) $(CFLAGS) $?

libhal_dataflash.so : *.o
	$(CC) -shared -o $@ *.o
	rm -f *.o
	
clean :
	rm -f $(TGT) *~	
        

 
