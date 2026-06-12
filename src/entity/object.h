#ifndef OBJECT_H 
#define OBJECT_H


extern "C" {
    #include "types.h"
    #include "render/render.h"
    #include "math/math3d.h"
    #include "load/load.h"
}

#include "entity/entity.h"

class Object: public Entity{
    public:
        bool gravity;
        vec3 velocity;
        float min_x, max_x;
        float min_y, max_y;
        float min_z, max_z;
        bool colliding;

        Object(int id, figure* m_figure, point start_pos, uint32_t color, bool gravity, float scale);
        void update_hitbox(int precision);
        void update_physics(float dt);
        void draw_hitbox(uint32_t* pixels, mat4x4 proj_mat, mat4x4 view_mat);

};


#endif