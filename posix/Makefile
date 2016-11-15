INC=-I. -I..
LIB=-L.. -lscheme -lm -lpthread
CC=cc
CFLAGS=-O $(INC)

OBJS = posix_file.o posix_proc.o posix_popen.o

libposix.a: $(OBJS)
	$(AR) rv $@ $^

main: posixscheme

posixscheme: libposix.a main.o
	$(CC) $(CFLAGS) -o $@ main.o $(LIB)

clean:
	rm -f $(OBJS) main.o libposix.a posixscheme

.PHONY: clean main
