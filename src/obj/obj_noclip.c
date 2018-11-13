#include "obj_noclip.h"

#include "../graphics/graphics.h"
#include "../event.h"
#include "../mem.h"
#include "../config.h"

typedef struct {
    int debug;
} obj_noclip;

void obj_noclip_init(ib_object* p) {
    obj_noclip* self = p->d = ib_malloc(sizeof *self);

    self->debug = ib_config_get_int("debug", 0);

    if (self->debug) {
        ib_object_subscribe(p, IB_EVT_DRAW);
    }
}

void obj_noclip_evt(ib_event* e, ib_object* obj) {
    ib_color c = { 0xFF, 0x00, 0x00, 0xAA };

    ib_graphics_opt_reset();
    ib_graphics_opt_color(c);
    ib_graphics_prim_outline(obj->pos, obj->size);
}

void obj_noclip_destroy(ib_object* p) {
    ib_free(p->d);
}
