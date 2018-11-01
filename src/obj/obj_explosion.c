#include "obj_explosion.h"
#include "../graphics.h"
#include "../mem.h"
#include "../event.h"
#include "../sprite.h"

#define OBJ_EXPLOSION_TEX IB_GRAPHICS_TEXFILE("explosion")

typedef struct {
    ib_sprite* spr;
    int subu, subd;
} obj_explosion;

static int obj_explosion_evt(ib_event* e, void* d);

void obj_explosion_init(ib_object* p) {
    obj_explosion* self = p->d = ib_malloc(sizeof *self);

    self->spr = ib_sprite_alloc_animated(OBJ_EXPLOSION_TEX, 64, 64, 30, 1);
    ib_sprite_anim_start(self->spr);

    self->subu = ib_event_subscribe(IB_EVT_UPDATE, obj_explosion_evt, p);
    self->subd = ib_event_subscribe(IB_EVT_DRAW, obj_explosion_evt, p);

    ib_event_add(IB_EVT_GAME_EXPLOSION, p, sizeof *p); /* notify other game objects something exploded */
    /* in the future, this event could be replaced with an "area damage" event to make things more concise and reusable */
}

int obj_explosion_evt(ib_event* e, void* d) {
    ib_object* obj = d;
    obj_explosion* self = obj->d;

    switch (e->type) {
    case IB_EVT_UPDATE:
        ib_sprite_update(self->spr);
        if (!self->spr->playing) { /* after the oneshot is over destroy the explosion object */
            ib_world_destroy_object(obj);
            return 0;
        }
        break;
    case IB_EVT_DRAW:
    {
        /* draw the explosion sprite above the explosion point */
        ib_graphics_point spos = obj->pos;
        spos.x -= self->spr->fw / 2;
        spos.y -= self->spr->fh;
        ib_graphics_draw_sprite(self->spr, spos);
    }
    break;
    }

    return 0;
}

void obj_explosion_destroy(ib_object* p) {
    obj_explosion* self = p->d;

    ib_sprite_free(self->spr);

    ib_event_unsubscribe(self->subu);
    ib_event_unsubscribe(self->subd);
}
