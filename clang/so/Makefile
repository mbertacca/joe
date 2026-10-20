CC=cc
#CFLAGS=-g -Wall -rdynamic
CFLAGS=-Ofast -Wall -fPIC
INCL=-I../include

BIN=../bin
EGLIB=libtimeobjs.so
PXLIB=libproxy.so

all:$(BIN)/$(EGLIB) $(BIN)/$(PXLIB)

$(BIN)/$(EGLIB):timeobjs.c
	$(CC) $(INCL) -fPIC -g -shared -o $(BIN)/$(EGLIB) -Wl,-soname,$(EGLIB) timeobjs.c

$(BIN)/$(PXLIB):proxy.c
	$(CC) $(INCL) -fPIC -g -shared -o $(BIN)/$(PXLIB) -Wl,-soname,$(PXLIB) proxy.c

.c.o:
	$(CC) $(INCL) -c $(CFLAGS) $(INCL) $<

clean:
	rm -f $(BIN)/$(EGLIB) 

eglib:
