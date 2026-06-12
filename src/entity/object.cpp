#include "entity/object.h"


Object::Object(int id, figure* m_figure, point start_pos, uint32_t color, bool gravity, float scale)
    : Entity(id, m_figure, start_pos, color, scale){
    this->gravity = gravity;
    this->velocity = (vec3){0.0f, 0.0f, 0.0f};
    this->colliding = false;
}

void Object::update_hitbox(int precision){
    if (m_figure->n_vertices == 0) return;

    min_x = max_x = transformed_vertices[0].x;
    min_y = max_y = transformed_vertices[0].y;
    min_z = max_z = transformed_vertices[0].z;

    for (int i = 1; i < m_figure->n_vertices; i++) {
        if (transformed_vertices[i].x < min_x) min_x = transformed_vertices[i].x;
        if (transformed_vertices[i].x > max_x) max_x = transformed_vertices[i].x;
        
        if (transformed_vertices[i].y < min_y) min_y = transformed_vertices[i].y;
        if (transformed_vertices[i].y > max_y) max_y = transformed_vertices[i].y;
        
        if (transformed_vertices[i].z < min_z) min_z = transformed_vertices[i].z;
        if (transformed_vertices[i].z > max_z) max_z = transformed_vertices[i].z;
    }
}

void Object::update_physics(float dt){
    if(gravity && !colliding){
        vec3 gravity = {0, +0.00098f, 0};
        gravity.x *= dt;
        gravity.y *= dt;
        gravity.z *= dt;
        velocity = vec3_add(gravity, velocity);
        position = vec3_add(velocity, position);
    }
}

void Object::draw_hitbox(uint32_t* pixels, mat4x4 proj_mat, mat4x4 view_mat){
    point world_points[8] = {
        (point){min_x, min_y, min_z}, // p0
        (point){max_x, min_y, min_z}, // p1
        (point){max_x, max_y, min_z}, // p2
        (point){min_x, max_y, min_z}, // p3
        (point){min_x, min_y, max_z}, // p4
        (point){max_x, min_y, max_z}, // p5
        (point){max_x, max_y, max_z}, // p6
        (point){min_x, max_y, max_z}  // p7
    };

    point view_points[8];
    for(int i = 0; i < 8; i++){
        view_points[i] = mat4x4_mul_vec3(view_mat, world_points[i]);
    }

    uint32_t red_color = 0xFFFF0000;

    int edges[12][2] = {
        {0,1}, {1,2}, {2,3}, {3,0}, 
        {4,5}, {5,6}, {6,7}, {7,4}, 
        {0,4}, {1,5}, {2,6}, {3,7}  
    };

    for(int i = 0; i < 12; i++){
        draw_clipped_edge(pixels, view_points[edges[i][0]],
        view_points[edges[i][1]], proj_mat, red_color);
    }
}