CC=cc
#CFLAGS=-g -Wall -rdynamic
CFLAGS=-Og -Wall -pedantic
INCL=-I../objmng -I../include
TARGET=../bin/joe
LIBS=-Wl,-rpath=\$$ORIGIN/. -L../bin -ljoeobj -lbangso -ldl

#all:joec testlib.so
OBJS=joec.o

$(TARGET):$(OBJS)
	cc $(CFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

.c.o:
	$(CC) -c $(CFLAGS) $(INCL) $<

clean:
	rm -f $(TARGET) *.o
