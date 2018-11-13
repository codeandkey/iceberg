#include "obj_grenade.h"
#include "../mem.h"
#include "../event.h"
#include "../timer.h"
#include "../graphics/graphics.h"

#define IB_GRENADE_TEXTURE IB_TEXTURE_FILE("grenade")

typedef struct {
    ib_sprite* spr;
    int vx, vy, vz, z; /* fake 3d kinda */
    int delay;
    float angle;
    ib_timepoint timer;
    int subd, subu;
} obj_grenade;

static int obj_grenade_evt(ib_event* e, void* d);

void obj_grenade_init(ib_object* p) {
    obj_grenade* self = p->d = ib_malloc(sizeof *self);
    self->spr = ib_sprite_alloc(IB_GRENADE_TEXTURE, 6, 3, 100);
    ib_sprite_start(self->spr);

    self->vx = ib_object_get_prop_int(p, "vx", 0);
    self->vy = ib_object_get_prop_int(p, "vy", 0);
    self->vz = ib_object_get_prop_int(p, "vz", 2);
    self->z = ib_object_get_prop_int(p, "z", 5);
    self->delay = ib_object_get_prop_int(p, "delay", 3000);
    self->timer = ib_timer_point();
    self->angle = 0.0f;

    self->subd = ib_event_subscribe(IB_EVT_DRAW, obj_grenade_evt, p);
    self->subu = ib_event_subscribe(IB_EVT_UPDATE, obj_grenade_evt, p);
}

int obj_grenade_evt(ib_event* e, void* d) {
    ib_object* obj = d;
    obj_grenade* self = obj->d;

    switch (e->type) {
    case IB_EVT_UPDATE:
        self->angle += 0.1f;
        obj->pos.x += self->vx;
        obj->pos.y += self->vy;

        if (ib_timer_ms(self->timer) >= self->delay) {
            /* should explode. destroy the grenade object */
            ib_world_create_object("explosion", NULL, NULL, obj->pos, obj->size, 0.0f, 1);
            ib_world_destroy_object(obj);
            return 0;
        }
        break;
    case IB_EVT_DRAW:
        ib_graphics_opt_reset();
        ib_graphics_opt_rot(self->angle);
        ib_graphics_tex_draw_sprite(self->spr, obj->pos);
        break;
    }

    return 0;
}

void obj_grenade_destroy(ib_object* p) {
    obj_grenade* self = p->d;
    ib_event_unsubscribe(self->subu);
    ib_event_unsubscribe(self->subd);
    ib_sprite_free(self->spr);
}
