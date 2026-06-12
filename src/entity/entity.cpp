#include "entity.h"
#include <stdio.h>

Entity::Entity(int id, figure* m_figure, point start_pos, uint32_t color, float scale) {
    this->id = id;
    this->m_figure = m_figure;
    position = start_pos;
    orientation = {0.0f, 0.0f, 0.0f};
    this->color = color;
    this->scale = scale;
}

int Entity::load_figure(char* file){
    return load_model(m_figure, &transformed_vertices, file);
}

void Entity::rotate(float delta_x, float delta_y){
    orientation.x += delta_x;
    orientation.y += delta_y;
}

void Entity::draw(uint32_t* pixels, float* z_buffer, mat4x4 proj_mat, mat4x4 view_mat, vec3 light, vec3 camera_pos){
    rotate_figure(m_figure, transformed_vertices, orientation);
    traslate_figure(transformed_vertices, m_figure->n_vertices, position);
    resize_figure(transformed_vertices, m_figure->n_vertices, scale);
    calc_triangles_aliniation(m_figure, transformed_vertices, camera_pos);
    draw_triangles(pixels, z_buffer, m_figure, transformed_vertices, proj_mat, view_mat, light);
    /*
    #ifndef DEBUG
        draw_triangles_edges(pixels, m_figure, proj_mat);
    #endif
    */
}



