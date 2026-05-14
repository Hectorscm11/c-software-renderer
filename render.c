#include <stdlib.h>
#include <math.h>
#include "render.h"
#include "math3d.h"

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

void scale_point(point* p){
    p->x = (p->x + 1.0f) * 0.5f * (float)WIDTH;
    p->y = (p->y + 1.0f) * 0.5f * (float)HEIGHT;
}

uint32_t calc_color_brightness(uint32_t color, float light_aliniation){
    float brightness = -light_aliniation;

    if (brightness < 0.2f) {
        brightness = 0.2f;
    }

    uint8_t a = (color >> 24) & 0xFF;
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    r = (uint8_t)(r * brightness);
    g = (uint8_t)(g * brightness);
    b = (uint8_t)(b * brightness);

    return (a << 24) | (r << 16) | (g << 8) | b;
}

void draw_triangle(uint32_t* pixels, float* z_buffer, figure* figure, triangle* triangle, mat4x4 proj_mat, mat4x4 view_mat, vec3 light_origin, uint32_t color){

    float light_aliniation = calc_triangle_aliniation(figure, triangle, light_origin);
    color = calc_color_brightness(color, light_aliniation);

    point p0 = mat4x4_mul_vec3(view_mat, figure->transformed_vertices[triangle->a]);
    point p1 = mat4x4_mul_vec3(view_mat, figure->transformed_vertices[triangle->b]);
    point p2 = mat4x4_mul_vec3(view_mat, figure->transformed_vertices[triangle->c]);

    p0 = mat4x4_mul_vec3(proj_mat, p0);
    p1 = mat4x4_mul_vec3(proj_mat, p1);
    p2 = mat4x4_mul_vec3(proj_mat, p2);

    scale_point(&p0);
    scale_point(&p1);
    scale_point(&p2);

    p0.x = (int)roundf(p0.x);
    p0.y = (int)roundf(p0.y);
    p1.x = (int)roundf(p1.x); 
    p1.y = (int)roundf(p1.y);
    p2.x = (int)roundf(p2.x); 
    p2.y = (int)roundf(p2.y);

    //z-buffer
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

void draw_triangles_edges(uint32_t* pixels,figure* figure, mat4x4 mat){
    for(int i = 0; i < figure->n_triangles; i++){
        if(figure->triangles[i].visible == 1){
                
            point vA = figure->transformed_vertices[figure->triangles[i].a];
            point vB = figure->transformed_vertices[figure->triangles[i].b];
            point vC = figure->transformed_vertices[figure->triangles[i].c];

            point pA, pB, pC;
            
            pA = (point)mat4x4_mul_vec3(mat, vA);
            pB = (point)mat4x4_mul_vec3(mat, vB);
            pC = (point)mat4x4_mul_vec3(mat, vC);

            
            scale_point(&pA);
            scale_point(&pB);
            scale_point(&pC);


            draw_line(pixels, &pA, &pB, 0xFF00FF00); 
            draw_line(pixels, &pB, &pC, 0xFF00FF00);
            draw_line(pixels, &pC, &pA, 0xFF00FF00);
            
        }
    }
}



void draw_edges(uint32_t* pixels, point* vertices, edge* edges, int n_edges, mat4x4 mat, uint32_t color){
    point projected_points[100];
    for(int i = 0; i < 8; i++){
        projected_points[i] = (point)mat4x4_mul_vec3(mat, vertices[i]);
    }

    for(int i = 0; i < n_edges; i++){
        draw_line(pixels, &projected_points[edges[i].a], &projected_points[edges[i].b],color);
    }

}


void draw_horizontal_line(uint32_t* pixels, float* z_buffer, int y,int x_left,int x_rigth ,float inv_z_left,float inv_z_rigth ,uint32_t color){
    if (y < 0 || y >= HEIGHT || x_left >= x_rigth || x_rigth <= 0 || x_left >= WIDTH) {
        return;
    }

    float z_step = (inv_z_rigth - inv_z_left) / (float)(x_rigth - x_left);
    float current_inv_z = inv_z_left;

    if (x_left < 0) {
        current_inv_z += z_step * (float)(-x_left); 
        x_left = 0;                                
    }

    if (x_rigth > WIDTH) {
        x_rigth = WIDTH;
    }

    int index = y * WIDTH + x_left;
    for (int i = x_left; i < x_rigth; i++) {
        if (current_inv_z > z_buffer[index]) {
            z_buffer[index] = current_inv_z;
            pixels[index] = color; 
        }
        
        current_inv_z += z_step; 
        index++;       
    }       
}

void draw_triangles(uint32_t* pixels, float* z_buffer, figure* figure, mat4x4 proj_mat, mat4x4 view_mat, vec3 light_origin){
    for(int i = 0; i < figure->n_triangles; i++){
        if(figure->triangles[i].visible == 1) draw_triangle(pixels, z_buffer, figure, &figure->triangles[i], proj_mat, view_mat, light_origin, 0xFFFFFFFF);
    }
}



mat4x4 init_projection_matrix(float fov_degrees, float aspect_ratio, float z_near, float z_far) {
    mat4x4 mat = {0};
    
    float fov_rad = 1.0f / tanf(fov_degrees * 0.5f / 180.0f * 3.14159f);
    
    mat.m[0][0] = aspect_ratio * fov_rad;
    mat.m[1][1] = fov_rad;
    mat.m[2][2] = z_far / (z_far - z_near);
    mat.m[3][2] = (-z_far * z_near) / (z_far - z_near);
    mat.m[2][3] = 1.0f;
    mat.m[3][3] = 0.0f;
    
    return mat;
}

mat4x4 matrix_make_lookat(vec3 pos, vec3 target, vec3 up) {
    vec3 forward;
    forward.x = target.x - pos.x;
    forward.y = target.y - pos.y;
    forward.z = target.z - pos.z;
    forward = vec3_normalize(forward);

    vec3 right = vec3_cross(up, forward);
    right = vec3_normalize(right);

    
    vec3 new_up = vec3_cross(forward, right);

    mat4x4 m;
    
    m.m[0][0] = right.x;   m.m[0][1] = new_up.x;   m.m[0][2] = forward.x;   m.m[0][3] = 0.0f;
    m.m[1][0] = right.y;   m.m[1][1] = new_up.y;   m.m[1][2] = forward.y;   m.m[1][3] = 0.0f;
    m.m[2][0] = right.z;   m.m[2][1] = new_up.z;   m.m[2][2] = forward.z;   m.m[2][3] = 0.0f;
    
    m.m[3][0] = -(right.x * pos.x + right.y * pos.y + right.z * pos.z);
    m.m[3][1] = -(new_up.x * pos.x + new_up.y * pos.y + new_up.z * pos.z);
    m.m[3][2] = -(forward.x * pos.x + forward.y * pos.y + forward.z * pos.z);
    m.m[3][3] = 1.0f;

    return m;
}