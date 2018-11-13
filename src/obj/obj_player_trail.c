#include "obj_player_trail.h"

#include "../graphics/graphics.h"
#include "../event.h"
#include "../mem.h"

#define OBJ_PLAYER_TRAIL_TEX IB_TEXTURE_FILE("trail")
#define OBJ_PLAYER_TRAIL_ALPHA_DEC 0.2f

typedef struct {
    ib_texture* tex;
    float alpha;
} obj_player_trail;

void obj_player_trail_init(ib_object* p) {
    /* TODO: change player trail sprite based on direction of blink */
    obj_player_trail* self = p->d = ib_malloc(sizeof *self);

    self->tex = ib_graphics_get_texture(OBJ_PLAYER_TRAIL_TEX);

    ib_object_subscribe(p, IB_EVT_UPDATE);
    ib_object_subscribe(p, IB_EVT_DRAW);

    self->alpha = 0.8f;
}

void obj_player_trail_evt(ib_event* e, ib_object* obj) {
    obj_player_trail* self = obj->d;

    switch (e->type) {
    case IB_EVT_UPDATE:
        if ((self->alpha -= OBJ_PLAYER_TRAIL_ALPHA_DEC) < 0.0f) {
            ib_world_destroy_object(obj);
            return;
        }
        break;
    case IB_EVT_DRAW:
        ib_graphics_opt_reset();
        ib_graphics_opt_alpha(self->alpha);
        ib_graphics_tex_draw_ex(self->tex, obj->pos, obj->size);
        break;
    }
}

void obj_player_trail_destroy(ib_object* p) {
    obj_player_trail* self = p->d;

    ib_graphics_drop_texture(self->tex);

    ib_free(self);
}
