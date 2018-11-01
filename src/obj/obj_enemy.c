#include "obj_enemy.h"

#include "../event.h"
#include "../graphics.h"
#include "../log.h"
#include "../mem.h"
#include "../sprite.h"

#define OBJ_ENEMY_TEX IB_GRAPHICS_TEXFILE("enemy")

typedef struct {
    ib_sprite* spr;
    int aitype; /* TODO: Implement multiple variants of AI (ideally passive/aggressive/defensive) */ 
    int subd, subu; 
    ib_graphics_point base_pos, base_size;
    int collision_result; /* enemys need collision too right? #equalopportunity */
} obj_enemy;

static int obj_enemy_evt(ib_event* e, void* d); 

/* init */
void obj_enemy_init(ib_object* p) {
    obj_enemy* self = p->d = ib_malloc(sizeof *self);

    self->aitype = ib_object_get_prop_int(p, "enemyai_type", 0);
    self->spr = ib_sprite_alloc(OBJ_ENEMY_TEX);
    self->subd = ib_event_subscribe(IB_EVT_DRAW, obj_enemy_evt, p);
    self->subu = ib_event_subscribe(IB_EVT_UPDATE, obj_enemy_evt, p);

    ib_ok("initialized obj_enemy with enemeyai_type=%d", self->aitype);
}

/* Destroy func */
void obj_enemy_destroy(ib_object* p) {
    obj_enemy* self = p->d;

    ib_sprite_free(self->spr);
    ib_event_unsubscribe(self->subd);
    ib_event_unsubscribe(self->subu);

    ib_free(self);
}

/* Event handling */
int obj_enemy_evt(ib_event* e, void* d) {
    ib_object* obj = d; 
    obj_enemy* self = obj->d; 

    switch(e->type) {
    case IB_EVT_UPDATE:
    /* TODO: Enemy AI */		
	break;
    case IB_EVT_DRAW:
	ib_graphics_set_space(IB_GRAPHICS_WORLDSPACE);
        ib_graphics_draw_sprite(self->spr, obj->pos);
        break;
    }

    return 0;
}
