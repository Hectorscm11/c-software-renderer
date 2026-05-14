#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include "types.h"


void draw_line(uint32_t* pixels, point *a, point *b, uint32_t color);
void draw_triangles_edges(uint32_t* pixels, figure* figure, mat4x4 mat);
void draw_edges(uint32_t* pixels, point* vertices, edge* edges, int n_edges, mat4x4 mat, uint32_t color);
void draw_triangles(uint32_t* pixels, float* z_buffer, figure* figure, mat4x4 proj_mat, mat4x4 view_mat, vec3 light_origin);
void draw_horizontal_line(uint32_t* pixels, float* z_buffer, int y,int x_left,int x_rigth ,float inv_z_left,float inv_z_rigth ,uint32_t color);
mat4x4 matrix_make_lookat(vec3 pos, vec3 target, vec3 up);
mat4x4 init_projection_matrix(float fov_degrees, float aspect_ratio, float z_near, float z_far);

#endif 