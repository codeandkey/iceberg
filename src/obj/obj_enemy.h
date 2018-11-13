#ifndef OBJ_ENEMY
#define OBJ_ENEMY

#include "../world.h"

void obj_enemy_init(ib_object* p);
void obj_enemy_destroy(ib_object* p);
void obj_enemy_evt(ib_event* e, ib_object* p);

#endif
