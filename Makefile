CC = gcc
CXX = g++

CFLAGS = -Wall -Wextra -O3 -ffast-math -march=native -std=c99 -fopenmp -Isrc $(shell pkg-config --cflags sdl2)
CXXFLAGS = -Wall -Wextra -O3 -ffast-math -march=native -std=c++17 -fopenmp -Isrc $(shell pkg-config --cflags sdl2)

LIBS = $(shell pkg-config --libs sdl2) -lm
TARGET = renderer

C_SRC = src/math/math3d.c src/render/render.c src/load/load.c
CPP_SRC = src/core/main.cpp src/entity/entity.cpp src/physics/physics.cpp

OBJ = $(C_SRC:.c=.o) $(CPP_SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LIBS) -fopenmp

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)