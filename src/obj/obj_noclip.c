#include "obj_noclip.h"

#include "../graphics/graphics.h"
#include "../event.h"
#include "../mem.h"
#include "../config.h"

typedef struct {
    int debug;
    int subd;
} obj_noclip;

static int obj_noclip_evt(ib_event* e, void* d);

void obj_noclip_init(ib_object* p) {
    obj_noclip* self = p->d = ib_malloc(sizeof *self);

    self->debug = ib_config_get_int("debug", 0);

    if (self->debug) {
        self->subd = ib_event_subscribe(IB_EVT_DRAW, obj_noclip_evt, p);
    }
}

int obj_noclip_evt(ib_event* e, void* d) {
    ib_object* obj = d;
    ib_color c = { 0xFF, 0x00, 0x00, 0xAA };

    ib_graphics_opt_reset();
    ib_graphics_opt_color(c);
    ib_graphics_prim_outline(obj->pos, obj->size);

    return 0;
}

void obj_noclip_destroy(ib_object* p) {
    obj_noclip* self = p->d;

    if (self->debug) {
        ib_event_unsubscribe(self->subd);
    }

    ib_free(self);
}
