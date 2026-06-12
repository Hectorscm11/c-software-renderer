#include "physics.h"

char collision(triangle t1, triangle t2){
    return 1;
}

char check_aabb_collision(Entity* e1, Entity* e2){
    char overlap_x = (e1->min_x <= e2->max_x) && (e1->max_x >= e2->min_x);
    char overlap_y = (e1->min_y <= e2->max_y) && (e1->max_y >= e2->min_y);
    char overlap_z = (e1->min_z <= e2->max_z) && (e1->max_z >= e2->min_z);
    return overlap_x && overlap_y && overlap_z;
}

void resolver_colision(Entity* e1, Entity* e2){
    e1->colliding = true;
    e2->colliding = true;
}
