#include "entity/terrain.h"

Terrain::Terrain(int id, figure* m_figure, point start_pos, uint32_t color, float scale)
    : Entity(id, m_figure, start_pos, color, scale) 
{
    this->chunks = nullptr; 
    this->chunk_width = 0;
}

void Terrain::split_terrain(figure* figure, int n_chunks){

}
