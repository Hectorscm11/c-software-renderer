#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include "types.h"

int project(point *v, point* p_v);
void draw_line(uint32_t* pixels, point *a, point *b, uint32_t color);
void draw_triangles_edges(uint32_t* pixels, figure* figure);
void draw_edges(uint32_t* pixels, point* vertices, edge* edges, int n_edges, uint32_t color);
void draw_triangles(uint32_t* pixels, float* z_buffer, figure* figure);
void draw_horizontal_line(uint32_t* pixels, float* z_buffer, int y,int x_left,int x_rigth ,float inv_z_left,float inv_z_rigth ,uint32_t color);


#endif 