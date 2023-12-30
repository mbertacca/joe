CC=cc
#CFLAGS=-g -Wall -rdynamic
CFLAGS=-Ofast -Wall -fPIC
INCL=-I../include
LIBS=-L../bin -ljoeobj -lm
BIN=../bin
EGLIB=libjoemath.so

all:$(BIN)/$(EGLIB)

$(BIN)/$(EGLIB):joe_Math.c
	$(CC) $(INCL) -fPIC -g -shared -o $(BIN)/$(EGLIB) -Wl,-soname,$(EGLIB) joe_Math.c $(LIBS)

.c.o:
	$(CC) $(INCL) -c $(CFLAGS) $(INCL) $<

clean:
	rm -f $(BIN)/$(EGLIB) 

eglib:
