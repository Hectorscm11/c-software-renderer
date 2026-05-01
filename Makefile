CC = gcc
CFLAGS = -Wall -Wextra -O3 -ffast-math -march=native -std=c99 $(shell pkg-config --cflags sdl2)
LIBS = $(shell pkg-config --libs sdl2) -lm
TARGET = renderer
SRC = c-software-renderer.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)