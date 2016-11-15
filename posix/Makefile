INC=-I. -I..
LIB=-L.. -lscheme -lm -lpthread
CC=cc
CFLAGS=-O $(INC)
AR=ar

OBJS = posix_file.o posix_proc.o

libposix.a: $(OBJS)
	$(AR) rv $@ $^
posixscheme: libposix.a main.o
	$(CC) $(CFLAGS) -o posixscheme libposix.a main.o $(LIB)

clean:
	rm -f $(OBJS) main.o *~ posixscheme

