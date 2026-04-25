# Variables
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 $(shell pkg-config --cflags sdl2)
LIBS = $(shell pkg-config --libs sdl2)
TARGET = renderer
SRC = c-software-renderer.c

# Regla principal
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)