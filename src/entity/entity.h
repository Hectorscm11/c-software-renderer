
#ifndef ENTITY_H 
#define ENTITY_H


extern "C" {
    #include "types.h"
    #include "render/render.h"
    #include "math/math3d.h"
    #include "load/load.h"
}

class Entity {
public:
    int id;
    point position;
    vec3 orientation;
    bool gravity;
    figure* m_figure;
    point* transformed_vertices;
    uint32_t color;
    vec3 velocity;
    float scale;
    float min_x, max_x;
    float min_y, max_y;
    float min_z, max_z;
    bool colliding;

    


    Entity(int id, figure* m_figure, point start_pos, uint32_t color, bool gravity, float scale);
    int load_figure(char* file);
    void rotate(float delta_x, float delta_y);
    void update_hitbox(int precision);
    void update_physics(float dt);
    void draw(uint32_t* pixels, float* z_buffer, mat4x4 proj_mat, mat4x4 view_mat, vec3 light, vec3 camera_pos);
    void draw_hitbox(uint32_t* pixels, mat4x4 proj_mat, mat4x4 view_mat);
};

#endif