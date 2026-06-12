#ifndef PHYSICS_H
#define PHYSICS_H

#include "types.h"
#include "entity/entity.h"

char collision(triangle t1, triangle t2);
char check_aabb_collision(Entity* e1, Entity* e2);
void resolver_colision(Entity* e1, Entity* e2);


#endif