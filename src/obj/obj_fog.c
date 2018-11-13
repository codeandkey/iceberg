#include "obj_fog.h"

#include "../graphics/graphics.h"
#include "../mem.h"
#include "../event.h"
#include "../log.h"

#define OBJ_FOG_TEXTURE IB_TEXTURE_FILE("mist")
#define OBJ_FOG_SPEED 10
#define OBJ_FOG_LAYERS 3
#define OBJ_FOG_PARALLAX_OFFSET 2 /* divisor of the topmost layer */

typedef struct {
    ib_texture* tex;
    float off;
} obj_fog;

void obj_fog_init(ib_object* p) {
    obj_fog* self = p->d = ib_malloc(sizeof *self);

    self->tex = ib_graphics_get_texture(OBJ_FOG_TEXTURE);
    self->off = 0.0f;

    ib_object_subscribe(p, IB_EVT_UPDATE);
    ib_object_subscribe(p, IB_EVT_DRAW);
}

void obj_fog_evt(ib_event* e, ib_object* obj) {
    obj_fog* self = obj->d;

    ib_ivec2 cpos, csize;

    switch (e->type) {
    case IB_EVT_UPDATE:
        self->off += OBJ_FOG_SPEED;
        break;
    case IB_EVT_DRAW:
        /* tricky render here, we want to tile the fog texture and still maintain multiple parallax levels */
        /* first compute the parallax offset (mod cwidth) for each layer */

        ib_graphics_get_camera(&cpos, &csize);
        ib_graphics_opt_reset();

        for (int i = 0; i < OBJ_FOG_LAYERS; ++i) {
            int cline = ((float) cpos.x + self->off) / (float) (OBJ_FOG_PARALLAX_OFFSET + i);

            /* mod spaces would not work well for this, do a quick loop */
            while (cline < cpos.x) cline += csize.x;
            while (cline >= cpos.x + csize.x) cline -= csize.x;

            ib_graphics_opt_alpha(1.0f / OBJ_FOG_LAYERS);

            /* render one to the left, one to the right at camera dimensions */
            ib_ivec2 clpos = cpos;
            clpos.x = cline;
            ib_graphics_tex_draw_ex(self->tex, clpos, csize);

            clpos.x -= csize.x;
            ib_graphics_tex_draw_ex(self->tex, clpos, csize);

            ib_graphics_opt_alpha(1.0f);

            ib_ivec2 a, b;
            a.x = cline;
            a.y = cpos.y;
            b.x = cline;
            b.y = cpos.y + csize.y;

            ib_graphics_prim_line(a, b);
        }

        break;
    }
}

void obj_fog_destroy(ib_object* p) {
    obj_fog* self = p->d;

    ib_graphics_drop_texture(self->tex);
    ib_free(self);
}
