GLOBUS_LOCATION=/usr

GLOBUS_INCLUDES=-I/usr/include/globus -I/usr/lib64/globus/include

INCLUDES=$(GLOBUS_INTERNAL_INCLUDES) $(GLOBUS_INCLUDES)
CFLAGS=-fPIC -ggdb3 $(GLOBUS_INTERNAL_INCLUDES) $(GLOBUS_INCLUDES)

all:: libtransfer_logger.so


libtransfer_logger.so: transfer_logger.o
	$(CC) -shared -o $@ $^

clean::
	rm -f transfer_logger.o

clobber:: clean
	rm -f libtransfer_logger.so
