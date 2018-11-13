#ifndef OBJ_EXPLOSION
#define OBJ_EXPLOSION

#include "../world.h"

void obj_explosion_init(ib_object* p);
void obj_explosion_destroy(ib_object* p);
void obj_explosion_evt(ib_event* e, ib_object* p);

#endif
