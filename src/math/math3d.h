#ifndef MATH3D_H
#define MATH3D_H

#include "types.h"

vec3 vec3_add(vec3 v1, vec3 v2);
vec3 vec3_sub(vec3 v1, vec3 v2);
vec3 vec3_cross(vec3 u, vec3 v);
float vec3_dot(vec3 v1, vec3 v2);
vec3 vec3_normalize(vec3 v);
point rotate_point(point p, float angle_x, float angle_y);
void rotate_figure(figure* figure, point* transformed_vertices, vec3 orientation);
void traslate_figure(point* transformed_vertices, int n_vertices, point position);
float calc_triangle_aliniation(point* transformed_vertices, triangle* tri, vec3 vec);
void calc_triangles_aliniation(figure* figure, point* transformed_vertices, vec3 camera_pos);
vec3 mat4x4_mul_vec3(mat4x4 m, vec3 i);

#endif 