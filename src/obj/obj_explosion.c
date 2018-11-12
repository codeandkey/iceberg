#include "obj_explosion.h"
#include "../graphics/graphics.h"
#include "../mem.h"
#include "../event.h"

#include <math.h>

#define OBJ_EXPLOSION_TEX IB_TEXTURE_FILE("explosion")
#define OBJ_EXPLOSION_SPARKS_MAX 30
#define OBJ_EXPLOSION_SPARKS_MIN 15
#define OBJ_EXPLOSION_SPARK_VEL_MAX 30
#define OBJ_EXPLOSION_SPARK_VEL_MIN 20
#define OBJ_EXPLOSION_SPARK_LEN_MAX 81
#define OBJ_EXPLOSION_SPARK_LEN_MIN 80
#define OBJ_EXPLOSION_SPARK_ALPHA_MAX 1.0f
#define OBJ_EXPLOSION_SPARK_ALPHA_MIN 0.2f
#define OBJ_EXPLOSION_SPARK_ALPHA_DECAY 2.0f

typedef struct {
    int vel, len;
    float x, y, ang, alpha; /* store position as floats vs normal points */
} obj_explosion_spark;

typedef struct {
    ib_sprite* spr;
    obj_explosion_spark sparks[OBJ_EXPLOSION_SPARKS_MAX];
    int num_sparks;
    int subu, subd;
} obj_explosion;

static int obj_explosion_evt(ib_event* e, void* d);

void obj_explosion_init(ib_object* p) {
    obj_explosion* self = p->d = ib_malloc(sizeof *self);

    self->spr = ib_sprite_alloc(OBJ_EXPLOSION_TEX, 64, 64, 30);

    ib_sprite_oneshot(self->spr, 1);
    ib_sprite_start(self->spr);

    self->subu = ib_event_subscribe(IB_EVT_UPDATE, obj_explosion_evt, p);
    self->subd = ib_event_subscribe(IB_EVT_DRAW, obj_explosion_evt, p);

    self->num_sparks = rand() % (OBJ_EXPLOSION_SPARKS_MAX - OBJ_EXPLOSION_SPARKS_MIN) + OBJ_EXPLOSION_SPARKS_MIN;

    for (int i = 0; i < self->num_sparks; ++i) {
        obj_explosion_spark* sp = self->sparks + i;

        sp->x = p->pos.x;
        sp->y = p->pos.y;
        sp->ang = ((rand() % 100) / 50.0f) * 3.141f;
        sp->vel = rand() % (OBJ_EXPLOSION_SPARK_VEL_MAX - OBJ_EXPLOSION_SPARK_VEL_MIN) + OBJ_EXPLOSION_SPARK_VEL_MIN;
        sp->len = rand() % (OBJ_EXPLOSION_SPARK_LEN_MAX - OBJ_EXPLOSION_SPARK_LEN_MIN) + OBJ_EXPLOSION_SPARK_LEN_MIN;
        sp->alpha = ((rand() % 100) / 100.0f) * (OBJ_EXPLOSION_SPARK_ALPHA_MAX - OBJ_EXPLOSION_SPARK_ALPHA_MIN) + OBJ_EXPLOSION_SPARK_ALPHA_MIN;
    }

    ib_event_add(IB_EVT_GAME_EXPLOSION, p, sizeof *p); /* notify other game objects something exploded */
    /* in the future, this event could be replaced with an "area damage" event to make things more concise and reusable */
}

int obj_explosion_evt(ib_event* e, void* d) {
    ib_object* obj = d;
    obj_explosion* self = obj->d;
    int should_die;

    switch (e->type) {
    case IB_EVT_UPDATE:
        should_die = 1;
        for (int i = 0; i < self->num_sparks; ++i) {
            /* update spark positions and values */
            obj_explosion_spark* sp = self->sparks + i;

            sp->x += cosf(sp->ang) * sp->vel;
            sp->y += sinf(sp->ang) * sp->vel;
            sp->alpha /= OBJ_EXPLOSION_SPARK_ALPHA_DECAY;

            if (sp->alpha > 0.01f) should_die = 0;
        }
        if (!self->spr->playing && should_die) { /* after the oneshot is over destroy the explosion object */
            ib_world_destroy_object(obj);
            return 0;
        }
        break;
    case IB_EVT_DRAW:
    {
        /* draw the explosion sprite above the explosion point */
        if (self->spr->playing) {
            ib_graphics_opt_reset();
            ib_ivec2 spos = obj->pos;
            spos.x -= self->spr->frame.x / 2;
            spos.y -= self->spr->frame.y;
            ib_graphics_tex_draw_sprite(self->spr, spos);
        }

        ib_graphics_opt_reset();
        ib_graphics_opt_blend(IB_GRAPHICS_BM_ADD);

        /* draw sparks */
        for (int i = 0; i < self->num_sparks; ++i) {
            obj_explosion_spark* sp = self->sparks + i;
            ib_ivec2 a, b;
            a.x = sp->x;
            a.y = sp->y;
            b.x = a.x + cosf(sp->ang) * sp->len;
            b.y = a.y + sinf(sp->ang) * sp->len;

            ib_color col = { 0xFF, 0xFF, 0x10, 0xFF };
            col.a = sp->alpha * 255.0f;

            ib_graphics_opt_color(col);
            ib_graphics_prim_line(a, b);
        }
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
