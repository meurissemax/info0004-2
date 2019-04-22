CC = g++
CFLAGS = -std=c++11 -Wall -Wextra
CFILES = src/parser.cpp src/painter-check.cpp
OUT = bin/painter-check

painter-check : src/parser.cpp src/painter-check.cpp
	$(CC) $(CFLAGS) $(CFILES) -o $(OUT)
