#ifndef TYPES_H
#define TYPES_H

#define WIDTH 1400
#define HEIGHT 900
#define PIXELS_PER_POINT 250

#define SWAP(type, a, b) do { type temp = a; a = b; b = temp; } while (0)

typedef struct point {
    float x;
    float y;
    float z;
} point, vec3;

typedef struct triangle {
    int a;
    int b;
    int c;
    char visible;
    float aliniation;
} triangle;

typedef struct edge {
    int a;
    int b;
} edge;

typedef struct figure {
    point* vertices;
    point* transformed_vertices;
    int n_vertices;
    triangle* triangles;
    int n_triangles;
    edge* edges;
    int n_edges;
    point position;
    float angle_x;
    float angle_y;
} figure;

#endif