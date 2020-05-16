CC = g++
CFLAGS = -std=c++14 -Wall -Wextra -Werror -O3
CFILES = src/geometry.cpp src/graphics.cpp src/painter.cpp src/parser.cpp
OUT = bin/painter

painter : $(CFILES)
	$(CC) $(CFLAGS) $(CFILES) -o $(OUT)
