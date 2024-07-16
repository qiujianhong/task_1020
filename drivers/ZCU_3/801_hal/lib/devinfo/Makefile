SRC:= $(wildcard *.c)
#TGT:= $(SRC:.c=.o)
TGT += libhal_devinfo.so

.PHONY := all
all : $(TGT)

%.o : %.c
	$(CC) $(CFLAGS) $?

libhal_devinfo.so : *.o
	$(CC) -shared -o $@ *.o
	rm -f *.o
	cp *.so $(LIB_TARGET)
	
clean :
	rm -f $(TGT) *~	
        

 
