#include "obj_player_trail.h"

#include "../graphics/graphics.h"
#include "../event.h"
#include "../mem.h"

#define OBJ_PLAYER_TRAIL_TEX IB_TEXTURE_FILE("trail")
#define OBJ_PLAYER_TRAIL_ALPHA_DEC 0.2f

typedef struct {
    ib_texture* tex;
    float alpha;
    int subd, subu;
} obj_player_trail;

static int obj_player_trail_evt(ib_event* e, void* d);

void obj_player_trail_init(ib_object* p) {
    /* TODO: change player trail sprite based on direction of blink */
    obj_player_trail* self = p->d = ib_malloc(sizeof *self);

    self->tex = ib_graphics_get_texture(OBJ_PLAYER_TRAIL_TEX);

    self->subu = ib_event_subscribe(IB_EVT_UPDATE, obj_player_trail_evt, p);
    self->subd = ib_event_subscribe(IB_EVT_DRAW, obj_player_trail_evt, p);

    self->alpha = 0.8f;
}

int obj_player_trail_evt(ib_event* e, void* d) {
    ib_object* obj = d;
    obj_player_trail* self = obj->d;

    switch (e->type) {
    case IB_EVT_UPDATE:
        if ((self->alpha -= OBJ_PLAYER_TRAIL_ALPHA_DEC) < 0.0f) {
            ib_world_destroy_object(obj);
            return 0;
        }
        break;
    case IB_EVT_DRAW:
        ib_graphics_opt_reset();
        ib_graphics_opt_alpha(self->alpha);
        ib_graphics_tex_draw_ex(self->tex, obj->pos, obj->size);
        break;
    }

    return 0;
}

void obj_player_trail_destroy(ib_object* p) {
    obj_player_trail* self = p->d;

    ib_event_unsubscribe(self->subu);
    ib_event_unsubscribe(self->subd);

    ib_graphics_drop_texture(self->tex);

    ib_free(self);
}
