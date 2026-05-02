#include <math.h>
#include "math3d.h"


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

void rotate_figure(figure* figure){
    for(int i = 0; i < figure->n_vertices; i++){
        figure->transformed_vertices[i] = rotate_point(figure->vertices[i], figure->angle_x, figure->angle_y);
        figure->transformed_vertices[i].z += 3.0f;
    }
}

void calc_triangle_visibility(figure* figure, vec3 camera_pos){
    for(int i = 0; i < figure->n_triangles; i++){
        triangle* tri = &figure->triangles[i];
        point vertex_a = figure->transformed_vertices[tri->a];
        point vertex_b = figure->transformed_vertices[tri->b];
        point vertex_c = figure->transformed_vertices[tri->c];

        vec3 u = vec3_sub((vec3)vertex_b, (vec3)vertex_a);
        vec3 v = vec3_sub(vertex_c, vertex_a);

        vec3 normal = vec3_cross(v, u);

        vec3 camera_vec = vec3_sub(vertex_a, camera_pos);

        float aliniation = vec3_dot(normal, camera_vec);

        if(aliniation < 0) tri->visible = 1;
        else tri->visible = 0;
    }
}