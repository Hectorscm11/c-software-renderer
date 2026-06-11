#include <math.h>
#include "math3d.h"

float vec3_length(vec3 v) {
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

vec3 vec3_normalize(vec3 v) {
    float len = vec3_length(v);
    
    
    if (len == 0.0f) {
        return v; 
    }
    
    vec3 result;
    result.x = v.x / len;
    result.y = v.y / len;
    result.z = v.z / len;
    
    return result;
}

vec3 vec3_add(vec3 v1, vec3 v2){
    return (vec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vec3 vec3_sub(vec3 v1, vec3 v2){
    vec3 v3;
    v3.x = v1.x - v2.x;
    v3.y = v1.y - v2.y;
    v3.z = v1.z - v2.z;
    return v3;
}

vec3 vec3_cross(vec3 u, vec3 v){
    vec3 w;
    w.x = (u.y * v.z) - (u.z * v.y);
    w.y = (u.z * v.x) - (u.x * v.z);
    w.z = (u.x * v.y) - (u.y * v.x);
    return w;
}

float vec3_dot(vec3 v1, vec3 v2){
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

point rotate_point(point p, float angle_x, float angle_y){
    point rotated = p;

    float cos_x = cos(angle_x);
    float sin_x = sin(angle_x);
    rotated.y = rotated.y * cos_x - rotated.z * sin_x;
    rotated.z = p.y * sin_x + rotated.z * cos_x;

    float cos_y = cos(angle_y);
    float sin_y = sin(angle_y);
    rotated.x = rotated.x * cos_y - rotated.z * sin_y;
    rotated.z = p.x * sin_y + rotated.z *cos_y;
    
    return rotated;
}

void rotate_figure(figure* figure, point* transformed_vertices, vec3 orientation){
    for(int i = 0; i < figure->n_vertices; i++){
        transformed_vertices[i] = rotate_point(figure->vertices[i], orientation.x, orientation.y);
    }
}

void traslate_figure(point* transformed_vertices, int n_vertices, point position){
    for(int i = 0; i < n_vertices; i++){
        transformed_vertices[i].x += position.x;
        transformed_vertices[i].y += position.y;
        transformed_vertices[i].z += position.z;
    }
}

float calc_triangle_aliniation(point* transformed_vertices, triangle* tri, vec3 vec) { 
    point vertex_a = transformed_vertices[tri->a];
    point vertex_b = transformed_vertices[tri->b];
    point vertex_c = transformed_vertices[tri->c];


    vec3 u = vec3_sub((vec3)vertex_b, (vec3)vertex_a);
    vec3 v = vec3_sub((vec3)vertex_c, (vec3)vertex_a); 

    vec3 normal = vec3_cross(u, v);
    normal = vec3_normalize(normal);
    
    float length = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
    normal.x /= length;
    normal.y /= length;
    normal.z /= length;

    vec3 camera_vec = vec3_sub((vec3)vertex_a, vec); 
    camera_vec = vec3_normalize(camera_vec);

    return vec3_dot(normal, camera_vec);
}

void calc_triangles_aliniation(figure* figure, point* transformed_vertices, vec3 camera_pos) {
    for(int i = 0; i < figure->n_triangles; i++) {
 
        triangle* tri = &figure->triangles[i]; 

        tri->aliniation = calc_triangle_aliniation(transformed_vertices, tri, camera_pos);

        if(tri->aliniation < 0) {
            tri->visible = 1;
        } else {
            tri->visible = 0;
        }
    }
}


vec3 mat4x4_mul_vec3(mat4x4 m, vec3 v){
    vec3 u;
    u.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0];
    u.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1];
    u.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2];

    float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];
    if (w != 0.0f) {
        u.x /= w;
        u.y /= w;
        u.z /= w;
    }

    return u;
}