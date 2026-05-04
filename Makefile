CC = gcc
CFLAGS = -Wall -Wextra -O3 -ffast-math -march=native -std=c99 $(shell pkg-config --cflags sdl2)
LIBS = $(shell pkg-config --libs sdl2) -lm
TARGET = renderer
SRC = main.c types.h math3d.h math3d.c render.h render.c load.h load.c
OBJ = main.c math3d.c render.c load.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)