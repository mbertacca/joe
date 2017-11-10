CC=cc
#CFLAGS=-g -Wall -rdynamic
CFLAGS=-g -Wall -fPIC
INCL=-I../include
OBJS=joe_BangSO.o

BIN=../bin
TARGET=libbangso.so 
EGLIB=libtimeobjs.so # just an example

all:$(BIN)/$(TARGET) $(BIN)/$(EGLIB)


$(BIN)/$(TARGET):$(OBJS)
	$(CC) -g -shared -o $(BIN)/$(TARGET) -Wl,-soname,$(TARGET) $(OBJS)

$(BIN)/$(EGLIB):timeobjs.c
	$(CC) $(INCL) -fPIC -g -shared -o $(BIN)/$(EGLIB) -Wl,-soname,$(EGLIB) timeobjs.c

.c.o:
	$(CC) $(INCL) -c $(CFLAGS) $(INCL) $<

clean:
	rm -f $(BIN)/$(EGLIB) $(BIN)/$(TARGET) $(OBJS) 

eglib:
