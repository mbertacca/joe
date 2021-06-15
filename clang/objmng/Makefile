CC=cc
CFLAGS=-g -fPIC -Wall -pedantic
TARGET=libjoeobj
LIB=../lib
BIN=../bin

INCL=-I../include -I.
OBJS=joe_Object.o \
     joe_Array.o \
     joe_ArrayList.o \
     joe_Float.o \
     joe_BigDecimal.o \
     joe_Integer.o \
     joe_String.o \
     joe_StringBuilder.o \
     joe_Boolean.o \
     joe_Bang.o \
     joe_Memory.o \
     joe_Message.o \
     joe_Block.o \
     joe_HashMap.o \
     joe_List.o \
     joe_Variable.o \
     joe_Assignment.o \
     joe_BreakException.o \
     joe_BreakBlockException.o \
     joe_BreakLoopException.o \
     joe_LoadScript.o \
     joe_Exception.o \
     ndecimal.o \
     joearray.o \
     joearrayscan.o \
     joeparser.o\
     joestrbuild.o \
     joetoken.o

all:$(LIB)/$(TARGET).a $(BIN)/$(TARGET).so

$(LIB)/$(TARGET).a:$(OBJS)
	ar r $(LIB)/$(TARGET).a $(OBJS)

$(BIN)/$(TARGET).so:$(OBJS)
	$(CC) -g -shared -o $(BIN)/$(TARGET).so -Wl,-soname,$(TARGET).so $(OBJS)


.c.o:
	$(CC) $(INCL) -c $(CFLAGS) $(INCL) $<

clean:
	rm -f $(BIN)/$(TARGET).so $(LIB)/$(TARGET).a $(OBJS) 

