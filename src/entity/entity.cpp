#include "entity.h"
#include <stdio.h>

Entity::Entity(figure* m_figure, point start_pos, uint32_t color, bool gravity) {
    this->m_figure = m_figure;
    position = start_pos;
    orientation = {0.0f, 0.0f, 0.0f};
    this->gravity = gravity;
    this->color = color;
    this->velocity = {0.0f, 0.0f, 0.0f};
}

int Entity::load_figure(char* file, float scale_factor){
    return load_model(m_figure, &transformed_vertices, file, scale_factor);
}

void Entity::rotate(float delta_x, float delta_y){
    orientation.x += delta_x;
    orientation.y += delta_y;
}

void Entity::update_physics(float dt){
    if(position.y < 0){
        vec3 gravity = {0, +0.00098f, 0};
        gravity.x *= dt;
        gravity.y *= dt;
        gravity.z *= dt;
        velocity = vec3_add(gravity, velocity);
        position = vec3_add(velocity, position);
    }
}

void Entity::draw(uint32_t* pixels, float* z_buffer, mat4x4 proj_mat, mat4x4 view_mat, vec3 light, vec3 camera_pos){
    rotate_figure(m_figure, transformed_vertices, orientation);
    traslate_figure(transformed_vertices, m_figure->n_vertices, position);
    calc_triangles_aliniation(m_figure, transformed_vertices, camera_pos);
    draw_triangles(pixels, z_buffer, m_figure, transformed_vertices, proj_mat, view_mat, light);
    /*
    #ifndef DEBUG
        draw_triangles_edges(pixels, m_figure, proj_mat);
    #endif
    */
}
