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

void obj_bg_init(ib_object* p) {
    obj_bg* self = p->d = ib_malloc(sizeof *self);

    char* imgpath = ib_object_get_prop_str(p, "img", OBJ_BG_DEF_IMG);
    self->img = ib_graphics_get_texture(imgpath);

    /* object resources / parameters ready, bind ourself to draw */
    ib_object_subscribe(p, IB_EVT_DRAW_BACKGROUND);
}

void obj_bg_evt(ib_event* e, ib_object* obj) {
    obj_bg* self = obj->d;

    ib_ivec2 cpos, csize;

    switch (e->type) {
    case IB_EVT_DRAW_BACKGROUND:
        ib_graphics_get_camera(&cpos, &csize);
        ib_graphics_opt_reset();
        ib_graphics_tex_draw_ex(self->img, cpos, csize);
        break;
    }
}

void obj_bg_destroy(ib_object* p) {
    obj_bg* self = (obj_bg*) p->d;
    ib_graphics_drop_texture(self->img);
    ib_free(self);
}
