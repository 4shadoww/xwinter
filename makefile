EXECUTABLE=snowfall
CC=g++
CFLAGS=-Wall -O3
LIBS=-lxcb -lxcb-ewmh -lcairo -lX11 -lXext

all: main

main: snowfall.cpp
	$(CC) snowfall.cpp $(CFLAGS) $(LIBS) -o $(EXECUTABLE)
