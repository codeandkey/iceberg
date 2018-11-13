#include "obj_enemy.h"

#include "../event.h"
#include "../graphics/graphics.h"
#include "../log.h"
#include "../mem.h"

#define OBJ_ENEMY_TEX IB_TEXTURE_FILE("enemy")

typedef struct {
    ib_sprite* spr;
    int aitype; /* TODO: Implement multiple variants of AI (ideally passive/aggressive/defensive) */
    ib_ivec2 base_pos, base_size;
    int collision_result; /* enemys need collision too right? #equalopportunity */
} obj_enemy;

/* init */
void obj_enemy_init(ib_object* p) {
    obj_enemy* self = p->d = ib_malloc(sizeof *self);

    self->aitype = ib_object_get_prop_int(p, "enemyai_type", 0);
    self->spr = ib_sprite_alloc(OBJ_ENEMY_TEX, 32, 32, 0);

    ib_object_subscribe(p, IB_EVT_DRAW);
    ib_object_subscribe(p, IB_EVT_UPDATE);

    ib_ok("initialized obj_enemy with enemeyai_type=%d", self->aitype);
}

/* Destroy func */
void obj_enemy_destroy(ib_object* p) {
    obj_enemy* self = p->d;

    ib_sprite_free(self->spr);
    ib_free(self);
}

/* Event handling */
void obj_enemy_evt(ib_event* e, ib_object* obj) {
    obj_enemy* self = obj->d;

    switch(e->type) {
    case IB_EVT_UPDATE:
        /* TODO: Enemy AI */
        break;
    case IB_EVT_DRAW:
        ib_graphics_opt_reset();
        ib_graphics_tex_draw_sprite(self->spr, obj->pos);
        break;
    }
}
