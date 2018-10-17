#include "obj_snow.h"

#include "../graphics.h"
#include "../event.h"
#include "../mem.h"

#define OBJ_SNOW_NUM_PARTS 128

typedef struct {
    float x, y, dx, dy;
    float rot, drot;
    int type;
} obj_snow_flake;

typedef struct {
    ib_graphics_texture* flakes[3];
    obj_snow_flake parts[OBJ_SNOW_NUM_PARTS];
} obj_snow;

static int obj_snow_evt(ib_event* e, void* d);

void obj_snow_init(ib_object* p) {
    obj_snow* d = p->d = ib_malloc(sizeof *d);

    d->flakes[0] = ib_graphics_get_texture(IB_GRAPHICS_TEXFILE("snowflake1"));
    d->flakes[1] = ib_graphics_get_texture(IB_GRAPHICS_TEXFILE("snowflake2"));
    d->flakes[2] = ib_graphics_get_texture(IB_GRAPHICS_TEXFILE("snowflake3"));

    int cx, cy;
    ib_graphics_get_camera(&cx, &cy);

    for (int i = 0; i < OBJ_SNOW_NUM_PARTS; ++i) {
        d->parts[i].x = rand() % IB_GRAPHICS_WIDTH + cx;
        d->parts[i].y = rand() % IB_GRAPHICS_HEIGHT + cy;
        d->parts[i].type = rand() % 3;
        d->parts[i].dx = rand() % 17 - 8;
        d->parts[i].dy = rand() % 25 + 20;
        d->parts[i].rot = ((rand() % 100) / 99.0f) * 3.141f * 2.0f;
        d->parts[i].drot = ((rand() % 100) / 99.0f) * 30.0f - 15.0f;
    }

    ib_event_subscribe(IB_EVT_DRAW, obj_snow_evt, d);
    ib_event_subscribe(IB_EVT_UPDATE, obj_snow_evt, d);
}

int obj_snow_evt(ib_event* e, void* ed) {
    obj_snow* d = ed;

    int cx, cy, vpw = IB_GRAPHICS_WIDTH, vph = IB_GRAPHICS_HEIGHT;
    ib_graphics_get_camera(&cx, &cy);

    switch (e->type) {
    case IB_EVT_UPDATE:
        {
            ib_event_update* u = e->evt;
            float dt_sec = u->dt / 1000.0f;

            for (int i = 0; i < OBJ_SNOW_NUM_PARTS; ++i) {
                d->parts[i].x += d->parts[i].dx * dt_sec;
                d->parts[i].y += d->parts[i].dy * dt_sec;
                d->parts[i].rot += d->parts[i].drot * dt_sec;

                if (d->parts[i].x + d->flakes[d->parts[i].type]->size.x < cx) d->parts[i].x += vpw;
                if (d->parts[i].y < cy) d->parts[i].y += vph;
                if (d->parts[i].x > cx + vpw) d->parts[i].x -= vpw;
                if (d->parts[i].y - d->flakes[d->parts[i].type]->size.y > cy + vph + 10) d->parts[i].y -= (vph + 10);
            }
        }
        break;
    case IB_EVT_DRAW:
            for (int i = 0; i < OBJ_SNOW_NUM_PARTS; ++i) {
                ib_graphics_point pos;

                pos.x = d->parts[i].x;
                pos.y = d->parts[i].y;

                ib_graphics_draw_texture_rot(d->flakes[d->parts[i].type], pos, d->parts[i].rot);
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
