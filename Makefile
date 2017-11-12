# makefile for capstone window manager
BIN = main 
CC = gcc

LIBS = -lX11 -lXpm

OBJS = buttonEvents.o configureEvents.o destroyEvents.o \
       displayEvents.o initCapstone.o keyEvents.o \
       motionEvents.o reparent.o main.o

all: $(OBJS)
	$(CC) $(OBJS) -o $(BIN) $(LIBS)

%.o: %.c
	$(CC) -c -o $@ $<

clean:
	rm -rf *.o $(BIN)
