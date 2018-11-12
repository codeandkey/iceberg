#include "obj_light.h"

#include "../mem.h"
#include "../event.h"
#include "../graphics/graphics.h"

#define OBJ_LIGHT_ALPHA_DEC 0.01f /* alpha decrement */
#define OBJ_LIGHT_BASE_ALPHA 0.7f /* resting alpha */
#define OBJ_LIGHT_FLICKER_ALPHA 0.8f /* flicker alpha */
#define OBJ_LIGHT_MIN_FLICKER_UPDATE 30 /* minimum updates before next flicker */
#define OBJ_LIGHT_MAX_FLICKER_UPDATE 50
#define OBJ_LIGHT_TEX IB_TEXTURE_FILE("light")

typedef struct {
    ib_texture* tex;
    float alpha;
    int next_flicker, subd, subu;
} obj_light;

static int obj_light_evt(ib_event* e, void* d);

void obj_light_init(ib_object* p) {
    obj_light* self = p->d = ib_malloc(sizeof *self);
    self->alpha = OBJ_LIGHT_BASE_ALPHA;
    self->next_flicker = 1;

    self->tex = ib_graphics_get_texture(OBJ_LIGHT_TEX);

    self->subd = ib_event_subscribe(IB_EVT_DRAW_WORLD_LIGHTS, obj_light_evt, p);
    self->subu = ib_event_subscribe(IB_EVT_UPDATE, obj_light_evt, p);
}

int obj_light_evt(ib_event* e, void* d) {
    ib_object* obj = d;
    obj_light* self = obj->d;

    switch (e->type) {
    case IB_EVT_UPDATE:
        if ((self->alpha -= OBJ_LIGHT_ALPHA_DEC) < OBJ_LIGHT_BASE_ALPHA) {
            self->alpha = OBJ_LIGHT_BASE_ALPHA;
        }
        if (!--self->next_flicker) {
            self->alpha = OBJ_LIGHT_FLICKER_ALPHA;
            self->next_flicker = rand() % (OBJ_LIGHT_MAX_FLICKER_UPDATE - OBJ_LIGHT_MIN_FLICKER_UPDATE) + OBJ_LIGHT_MIN_FLICKER_UPDATE;
        }
        break;
    case IB_EVT_DRAW_WORLD_LIGHTS:
        ib_graphics_opt_reset();
        ib_graphics_opt_blend(IB_GRAPHICS_BM_ADD);
        ib_graphics_opt_alpha(self->alpha);
        ib_graphics_tex_draw_ex(self->tex, obj->pos, obj->size);
        break;
    }

    return 0;
}

void obj_light_destroy(ib_object* p) {
    obj_light* self = p->d;

    ib_event_unsubscribe(self->subd);
    ib_event_unsubscribe(self->subu);

    ib_graphics_drop_texture(self->tex);
    ib_free(self);
}
