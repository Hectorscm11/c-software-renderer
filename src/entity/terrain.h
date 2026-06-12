#ifndef TERRAIN_H 
#define TERRAIN_H


extern "C" {
    #include "types.h"
    #include "render/render.h"
    #include "math/math3d.h"
    #include "load/load.h"
}

#include "entity/entity.h"

class Terrain: public Entity{
    public:
        chunk* chunks;
        int chunk_width;

        Terrain(int id, figure* m_figure, point start_pos, uint32_t color, float scale);
        void split_terrain(figure* figure, int n_chunks);

};


#endif