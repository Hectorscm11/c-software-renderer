
#ifndef MATH3D_H
#define MATH3D_H

#include "types.h"

vec3 vec3_sub(vec3 v1, vec3 v2);
vec3 vec3_cross(vec3 u, vec3 v);
float vec3_dot(vec3 v1, vec3 v2);
point rotate_point(point p, float angle_x, float angle_y);
void rotate_figure(figure* figure);
void calc_triangle_visibility(figure* figure, vec3 camera_pos);

#endif 