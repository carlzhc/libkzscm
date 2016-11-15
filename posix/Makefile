INC=-I. -I..
LIB=-L.. -lkzscm -lm -lpthread
CC=cc
CFLAGS=-O $(INC)

OBJS = scheme_posix_file.o scheme_posix_proc.o scheme_posix_popen.o

libscheme_posix.a: $(OBJS)
	$(AR) rv $@ $^

main: posixscheme

posixscheme: libscheme_posix.a main.o
	$(CC) $(CFLAGS) -o $@ main.o $(LIB)

clean:
	rm -f $(OBJS) main.o libscheme_posix.a posixscheme

.PHONY: clean main
