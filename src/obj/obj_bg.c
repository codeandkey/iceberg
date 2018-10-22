#include "obj_bg.h"

#include "../graphics.h"
#include "../event.h"
#include "../mem.h"
#include "../log.h"

#define OBJ_BG_DEF_PFX 0.5
#define OBJ_BG_DEF_PFY 0.5
#define OBJ_BG_DEF_IMG IB_GRAPHICS_TEXFILE("bg")
#define OBJ_BG_DEF_SCROLL_PER_SEC 1

typedef struct {
    ib_graphics_texture* img;
    double pfx, pfy;
    int sub_draw;
} obj_bg;

static int obj_bg_evt(ib_event* e, void* d);

void obj_bg_init(ib_object* p) {
    obj_bg* self = p->d = ib_malloc(sizeof *self);

    self->pfx = ib_object_get_prop_scalar(p, "pfx", OBJ_BG_DEF_PFX);
    self->pfy = ib_object_get_prop_scalar(p, "pfy", OBJ_BG_DEF_PFY);

    char* imgpath = ib_object_get_prop_str(p, "img", OBJ_BG_DEF_IMG);
    self->img = ib_graphics_get_texture(imgpath);

    /* object resources / parameters ready, bind ourself to draw */
    self->sub_draw = ib_event_subscribe(IB_EVT_DRAW_BACKGROUND, obj_bg_evt, self);
}

int obj_bg_evt(ib_event* e, void* d) {
    obj_bg* self = (obj_bg*) d;

    ib_graphics_point pos = {0}, screensize = {0};
    ib_graphics_set_space(IB_GRAPHICS_SCREENSPACE);
    ib_graphics_get_size(&screensize.x, &screensize.y);

    switch (e->type) {
    case IB_EVT_DRAW_BACKGROUND:
        ib_graphics_draw_texture_size(self->img, pos, screensize);
        break;
    }

    return 0;
}

void obj_bg_destroy(ib_object* p) {
    obj_bg* self = (obj_bg*) p->d;
    ib_graphics_drop_texture(self->img);
    ib_event_unsubscribe(self->sub_draw);
    ib_free(self);
}
