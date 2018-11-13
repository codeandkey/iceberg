#include "obj_snow.h"

#include "../graphics/graphics.h"
#include "../event.h"
#include "../mem.h"

/* perspective tricks
 * render one half of the snow above the world and the other half below it. */

/* making the above-world snow consistently collide with the ground uniformly is actually kinda a tough problem */

/* for this we can have an interesting algorithm:
 * for each snowflake select a y-coordinate to "drop" at
 * when we cross the y-coord we only drop if we're over land */

#define OBJ_SNOW_NUM_PARTS 81

typedef struct {
    float x, y, dx, dy;
    float rot, drot, alpha;
    int type;
    int y_drop, dropped, still;
} obj_snow_flake;

typedef struct {
    ib_texture* flakes[3];
    obj_snow_flake parts[OBJ_SNOW_NUM_PARTS];
} obj_snow;

static int obj_snow_evt(ib_event* e, void* d);

void obj_snow_init(ib_object* p) {
    obj_snow* d = p->d = ib_malloc(sizeof *d);

    d->flakes[0] = ib_graphics_get_texture(IB_TEXTURE_FILE("snowflake1"));
    d->flakes[1] = ib_graphics_get_texture(IB_TEXTURE_FILE("snowflake2"));
    d->flakes[2] = ib_graphics_get_texture(IB_TEXTURE_FILE("snowflake3"));

    ib_ivec2 cpos, csize;
    ib_graphics_get_camera(&cpos, &csize);

    for (int i = 0; i < OBJ_SNOW_NUM_PARTS; ++i) {
        d->parts[i].x = rand() % csize.x + cpos.x;
        d->parts[i].y = rand() % csize.y + cpos.y;
        d->parts[i].type = rand() % 3;
        d->parts[i].dx = (rand() % 10) * 2.0f / 9.0f - 1.0f;
        d->parts[i].dy = (rand() % 10) / 9.0f + 1.0f;
        d->parts[i].rot = ((rand() % 100) / 99.0f) * 3.141f * 2.0f;
        d->parts[i].drot = ((rand() % 100) / 99.0f) * 0.2f - 0.1f;
        d->parts[i].alpha = (rand() % 100) / 140.0f;
        d->parts[i].y_drop = csize.y + (rand() % csize.y);
        d->parts[i].dropped = 0;
        d->parts[i].still = 0;
    }

    ib_event_subscribe(IB_EVT_DRAW, obj_snow_evt, d);
    ib_event_subscribe(IB_EVT_UPDATE, obj_snow_evt, d);
}

int obj_snow_evt(ib_event* e, void* ed) {
    obj_snow* d = ed;

    ib_ivec2 cpos, csize;
    ib_graphics_get_camera(&cpos, &csize);

    switch (e->type) {
    case IB_EVT_UPDATE:
    {
        for (int i = 0; i < OBJ_SNOW_NUM_PARTS; ++i) {
            if (d->parts[i].still) {
                if (!--d->parts[i].still) {
                    /* reset dropped particle */
                    d->parts[i].y = -10;
                    d->parts[i].y_drop = cpos.y + rand() % (csize.y * 3 / 2);
                    d->parts[i].dropped = 0;
                    d->parts[i].x = rand() % csize.x;
                }
                continue;
            }

            d->parts[i].x += d->parts[i].dx;
            d->parts[i].y += d->parts[i].dy;
            d->parts[i].rot += d->parts[i].drot;

            if (d->parts[i].x + d->flakes[d->parts[i].type]->size.x < cpos.x) d->parts[i].x += csize.x;
            if (d->parts[i].x > cpos.x + csize.x) d->parts[i].x -= csize.x;

            if ((d->parts[i].y > d->parts[i].y_drop) && !d->parts[i].dropped) {
                ib_ivec2 p;
                p.x = d->parts[i].x;
                p.y = d->parts[i].y;
                d->parts[i].dropped = 1;

                if (ib_world_aabb(p, d->flakes[d->parts[i].type]->size)) {
                    d->parts[i].still = d->parts[i].dropped = 60;
                }
            }

            if (d->parts[i].y > cpos.y + csize.y) {
                d->parts[i].y = cpos.y - 10;
                d->parts[i].x = cpos.x + rand() % csize.x;
                d->parts[i].y_drop = cpos.y + rand() % (csize.y * 3 / 2);
                d->parts[i].dropped = 0;
            }
        }
    }
    break;
    case IB_EVT_DRAW:
        ib_graphics_opt_reset();

        for (int i = 0; i < OBJ_SNOW_NUM_PARTS; ++i) {
            ib_ivec2 pos;

            pos.x = d->parts[i].x;
            pos.y = d->parts[i].y;

            ib_texture* t = d->flakes[d->parts[i].type];

            float alp = d->parts[i].still ? d->parts[i].alpha * (float) d->parts[i].still / (float) d->parts[i].dropped : d->parts[i].alpha;

            ib_graphics_opt_alpha(alp);
            ib_graphics_opt_rot(d->parts[i].rot);

            ib_graphics_tex_draw_ex(t, pos, t->size);
        }
        break;
    }

    return 0;
}

void obj_snow_destroy(ib_object* p) {
    obj_snow* d = p->d;

    for (int i = 0; i < 3; ++i) {
        ib_graphics_drop_texture(d->flakes[i]);
    }
}
