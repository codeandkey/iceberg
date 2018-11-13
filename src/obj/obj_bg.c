#include "obj_bg.h"

#include "../graphics/graphics.h"
#include "../event.h"
#include "../mem.h"
#include "../log.h"

#define OBJ_BG_DEF_PFX 0.5
#define OBJ_BG_DEF_PFY 0.5
#define OBJ_BG_DEF_IMG IB_TEXTURE_FILE("bg")
#define OBJ_BG_DEF_SCROLL_PER_SEC 1

typedef struct {
    ib_texture* img;
    int sub_draw;
} obj_bg;

static int obj_bg_evt(ib_event* e, void* d);

void obj_bg_init(ib_object* p) {
    obj_bg* self = p->d = ib_malloc(sizeof *self);

    char* imgpath = ib_object_get_prop_str(p, "img", OBJ_BG_DEF_IMG);
    self->img = ib_graphics_get_texture(imgpath);

    /* object resources / parameters ready, bind ourself to draw */
    self->sub_draw = ib_event_subscribe(IB_EVT_DRAW_BACKGROUND, obj_bg_evt, self);
}

int obj_bg_evt(ib_event* e, void* d) {
    obj_bg* self = (obj_bg*) d;

    ib_ivec2 cpos, csize;

    switch (e->type) {
    case IB_EVT_DRAW_BACKGROUND:
        ib_graphics_get_camera(&cpos, &csize);
        ib_graphics_opt_reset();
        ib_graphics_tex_draw_ex(self->img, cpos, csize);
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
