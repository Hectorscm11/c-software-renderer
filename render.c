#include <stdlib.h>
#include <math.h>
#include "render.h"

static inline void draw_pixel(uint32_t* pixels, int x, int y, uint32_t color){
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        pixels[y * WIDTH + x] = color;
    }
}

static inline float interpolate(int y, int y0, int y1, float v0, float v1){
    if (y0 == y1) {
        return v0;
    }
    
    float t = (float)(y - y0) / (y1 - y0);
    
    return v0 + t * (v1 - v0);
}

void draw_triangle(uint32_t* pixels, float* z_buffer, figure* figure, triangle* triangle, uint32_t color){

    point p0, p1, p2;
    if (project(&figure->transformed_vertices[triangle->a], &p0) != 0 ||
        project(&figure->transformed_vertices[triangle->b], &p1) != 0 ||
        project(&figure->transformed_vertices[triangle->c], &p2) != 0) {
        return; 
    }


    float inv_z0 = 1.0 / figure->transformed_vertices[triangle->a].z;
    float inv_z1 = 1.0 / figure->transformed_vertices[triangle->b].z;
    float inv_z2 = 1.0 / figure->transformed_vertices[triangle->c].z;

    if (p0.y > p1.y) {
        SWAP(point, p0, p1);
        SWAP(float, inv_z0, inv_z1); 
    }
    if (p0.y > p2.y) {
        SWAP(point, p0, p2);
        SWAP(float, inv_z0, inv_z2);
    }
    if (p1.y > p2.y) {
        SWAP(point, p1, p2);
        SWAP(float, inv_z1, inv_z2);
    }

    int total_height = p2.y - p0.y;
    if (total_height == 0) return;

    for(int i = p0.y; i <= p2.y; i++){

        int is_second_half = (i > p1.y) || (p1.y == p0.y);

        int segment_height = is_second_half ? (p2.y - p1.y) : (p1.y - p0.y);
        if (segment_height == 0) continue;

        int x_long = (int)interpolate(i, p0.y, p2.y, p0.x, p2.x);
        float inv_z_long = interpolate(i, p0.y, p2.y, inv_z0, inv_z2);

        int x_sort;
        float inv_z_sort;

        if (!is_second_half) {
            //upper part
            x_sort = (int)interpolate(i, p0.y, p1.y, p0.x, p1.x);
            inv_z_sort = interpolate(i, p0.y, p1.y, inv_z0, inv_z1);
        }
        else {
            //lower part
            x_sort = (int)interpolate(i, p1.y, p2.y, p1.x, p2.x);
            inv_z_sort = interpolate(i, p1.y, p2.y, inv_z1, inv_z2);
        }


        int x_left = x_long;
        int x_rigth = x_sort;
        float inv_z_left = inv_z_long;
        float inv_z_rigth = inv_z_sort;


        if (x_left > x_rigth) {
            SWAP(int, x_left, x_rigth);
            SWAP(float, inv_z_left, inv_z_rigth);
        }

        draw_horizontal_line(pixels, z_buffer, i, x_left, x_rigth, inv_z_left, inv_z_rigth, color);

    }

}

void draw_line(uint32_t* pixels, point *a, point *b, uint32_t color){
    //Bresenham algorithm
    int x0 = a->x;
    int y0 = a->y;
    int x1 = b->x;
    int y1 = b->y;

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    int e2;
    while(1){
        draw_pixel(pixels, x0, y0, color);

        if(x0 == x1 && y0 == y1) break;

        e2 = 2 *err;

        if(e2 > -dy){
            err -=dy;
            x0 +=sx;
        }

        if(e2 < dx){
            err += dx;
            y0 +=sy;
        } 
    }
}

void draw_triangles_edges(uint32_t* pixels,figure* figure){
    for(int i = 0; i < figure->n_triangles; i++){
        if(figure->triangles[i].visible == 1){
                
            point vA = figure->transformed_vertices[figure->triangles[i].a];
            point vB = figure->transformed_vertices[figure->triangles[i].b];
            point vC = figure->transformed_vertices[figure->triangles[i].c];

            point pA, pB, pC;
            if (project(&vA, &pA) == 0 && 
                project(&vB, &pB) == 0 && 
                project(&vC, &pC) == 0) {
                
                draw_line(pixels, &pA, &pB, 0xFF00FF00); 
                draw_line(pixels, &pB, &pC, 0xFF00FF00);
                draw_line(pixels, &pC, &pA, 0xFF00FF00);
            }
        }
    }
}

int project(point* v, point* p_v) {
    if(v->z == 0){
        return -1;
    }
    
    p_v->x = (v->x / v->z) * PIXELS_PER_POINT + (WIDTH / 2);
    p_v->y = (v->y / v->z) * PIXELS_PER_POINT + (HEIGHT / 2);
    p_v->z = v->z;

    return 0;
}

void draw_edges(uint32_t* pixels, point* vertices, edge* edges, int n_edges, uint32_t color){
    point projected_points[100];
    for(int i = 0; i < 8; i++){
        if (project(&vertices[i], &projected_points[i]) == 0) {
            projected_points[i].z += 3.0f;
        }
    }

    for(int i = 0; i < n_edges; i++){
        draw_line(pixels, &projected_points[edges[i].a], &projected_points[edges[i].b],color);
    }

}


void draw_horizontal_line(uint32_t* pixels, float* z_buffer, int y,int x_left,int x_rigth ,float inv_z_left,float inv_z_rigth ,uint32_t color){
    int total_width = x_rigth - x_left;
    float t, inv_z_pixel;
    for(int i = x_left; i < x_rigth; i++){
        if(total_width == 0) t = 0;
        else t = (float)(i - x_left) / total_width;

        inv_z_pixel = inv_z_left + (t * (inv_z_rigth - inv_z_left));

        if(i >= 0 && i < WIDTH && y >= 0 && y < HEIGHT) {
            if(inv_z_pixel > z_buffer[(y * WIDTH) + i]) {
            draw_pixel(pixels, i, y, color);
            z_buffer[(y * WIDTH) + i] = inv_z_pixel;
            }
        }
    }
}

void draw_triangles(uint32_t* pixels, float* z_buffer, figure* figure, uint32_t color){
    for(int i = 0; i < figure->n_triangles; i++){
        if(figure->triangles[i].visible == 1) draw_triangle(pixels, z_buffer, figure, &figure->triangles[i], color);
    }
}




