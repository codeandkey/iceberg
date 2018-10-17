#include "obj_fog.h"

#include "../graphics.h"
#include "../event.h"
#include "../mem.h"

#define OBJ_FOG_IMAGE IB_GRAPHICS_TEXFILE("mist")
#define OBJ_FOG_DEPTH 3
#define OBJ_FOG_SPEED 14 /* speed of the highest parallax depth, back layers will be slower */

typedef struct {
    ib_graphics_texture* tex;
    int subu, subd;
    double scroll; /* constant scroll */
} obj_fog;

static int obj_fog_evt(ib_event* e, void* d);

void obj_fog_init(ib_object* p) {
    obj_fog* self = p->d = ib_malloc(sizeof *self);

    self->tex = ib_graphics_get_texture(OBJ_FOG_IMAGE);

    self->subu = ib_event_subscribe(IB_EVT_UPDATE, obj_fog_evt, self);
    self->subd = ib_event_subscribe(IB_EVT_DRAW, obj_fog_evt, self);
}

int obj_fog_evt(ib_event* e, void* d) {
    obj_fog* self = d;

    int cx, cy, cw, ch;
    ib_graphics_get_camera(&cx, &cy);
    ib_graphics_get_size(&cw, &ch);

    switch (e->type) {
    case IB_EVT_DRAW:
        /* movie magic here. treat each depth as a different parallax level relative to the camera */
        /* render parallax levels from back to front */
        /* we can fake a worldspace parallax by rendering in screenspace mod vpw which is a massive hack it's great */

        ib_graphics_set_space(IB_GRAPHICS_SCREENSPACE);
        ib_graphics_point pos;

        pos.y = 0;

        for (int level = OBJ_FOG_DEPTH - 1; level >= 0; --level) {
            int pos_absolute = ((int) ((self->scroll - cx) / (double) (level + 1))) % cw;
            if (pos_absolute < 0) pos_absolute += cw; /* get the position between 0 and cw */

            /* render a fog image on each side of the divider line to ensure it covers the screen */
            pos.x = pos_absolute;
            ib_graphics_draw_texture(self->tex, pos);

            pos.x -= self->tex->size.x;
            ib_graphics_draw_texture(self->tex, pos);
        }

        ib_graphics_set_space(IB_GRAPHICS_WORLDSPACE);
        break;
    case IB_EVT_UPDATE:
        {
            ib_event_update* u = e->evt;
            float dt_s = u->dt / 1000.0f;
            self->scroll += OBJ_FOG_SPEED * dt_s;
        }
        break;
    }

    return 0;
}

void obj_fog_destroy(ib_object* p) {
    obj_fog* self = p->d;
    ib_event_unsubscribe(self->subu);
    ib_event_unsubscribe(self->subd);
    ib_graphics_drop_texture(self->tex);
}
