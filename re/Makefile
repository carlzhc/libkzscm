# Things you might want to put in ENV:
# -DERRAVAIL		have utzoo-compatible error() function and friends
ENV=-I.. -I../gc/include

# Things you might want to put in TEST:
# -DDEBUG		debugging hooks
# -I.			regexp.h from current directory, not /usr/include
TEST=-I.

# Things you might want to put in PROF:
# -pg			profiler
PROF=

CFLAGS=-O2 $(ENV) $(TEST) $(PROF)
LDFLAGS=$(PROF) -L.. -lscheme -lpthread -lm

LIB=libregexp.a
OBJ=regexp.o regsub.o regerror.o scheme_regexp.o
TMP=dtr.tmp


$(LIB):	$(OBJ)
	$(AR) cr $(LIB) $(OBJ)

regexp.o:	regexp.c regexp.h regmagic.h
regsub.o:	regsub.c regexp.h regmagic.h
scheme_regexp.o: scheme_regexp.c    scheme_regexp.h

main: regexpscheme
regexpscheme: $(LIB) main.o
	$(CC) -o $@ main.o $(LDFLAGS)
clean:
	rm -f *.o core mon.out gmon.out timer.t.h copy try timer r.*
	rm -f residue rs.* re.1 rm.h re.h ch.soe ch.ps j badcom fig[012]
	rm -f ch.sml fig[12].ps $(LIB)
	rm -rf $(TMP) dtr.*

.PHONY: clean main
