CC=cc
#CFLAGS=-g -Wall -rdynamic
CFLAGS=-Ofast -Wall -fPIC
INCL=-I../include

BIN=../bin
EGLIB=libtimeobjs.so # just an example

all:$(BIN)/$(EGLIB)

$(BIN)/$(EGLIB):timeobjs.c
	$(CC) $(INCL) -fPIC -g -shared -o $(BIN)/$(EGLIB) -Wl,-soname,$(EGLIB) timeobjs.c

.c.o:
	$(CC) $(INCL) -c $(CFLAGS) $(INCL) $<

clean:
	rm -f $(BIN)/$(EGLIB) 

eglib:
