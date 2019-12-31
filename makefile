EXECUTABLE=xwinter
CC=g++
CFLAGS=-Wall -O3
LIBS=-lX11

all: main

main: xwinter.cpp
	$(CC) xwinter.cpp $(CFLAGS) $(LIBS) -o $(EXECUTABLE)
