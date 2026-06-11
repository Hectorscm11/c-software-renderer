
#ifndef ENTITY_H 
#define ENTITY_H


extern "C" {
    #include "types.h"
    #include "render/render.h"
    #include "math/math3d.h"
    #include "load/load.h"
}

class Entity {
private:
    point position;
    vec3 orientation;
    bool gravity;
    figure* m_figure;
    point* transformed_vertices;
    uint32_t color;

    

public:
    Entity(figure* m_figure, point start_pos, uint32_t color, bool gravity);
    int load_figure(char* file, float scale_factor);
    void rotate(float delta_x, float delta_y);
    void draw(uint32_t* pixels, float* z_buffer, mat4x4 proj_mat, mat4x4 view_mat, vec3 light, vec3 camera_pos);
};

#endif