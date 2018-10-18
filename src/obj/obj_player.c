#include "obj_player.h"

#include "../graphics.h"
#include "../event.h"
#include "../mem.h"
#include "../log.h"
#include "../input.h"

#define OBJ_PLAYER_TEXTURE IB_GRAPHICS_TEXFILE("player")
#define OBJ_PLAYER_SPEED_Y 2 /* !! 3 dimensions! */
#define OBJ_PLAYER_SPEED_X 3

typedef struct {
    ib_graphics_texture* tex;
    int subd, subu;
} obj_player;

static int obj_player_evt(ib_event* e, void* d);

void obj_player_init(ib_object* p) {
    obj_player* self = p->d = ib_malloc(sizeof *self);

    self->tex = ib_graphics_get_texture(OBJ_PLAYER_TEXTURE);
    self->subd = ib_event_subscribe(IB_EVT_DRAW, obj_player_evt, p);
    self->subu = ib_event_subscribe(IB_EVT_UPDATE, obj_player_evt, p);

    if (p->size.x != 32 || p->size.y != 32) ib_warn("your map player size is weird and I don't understand it (%dx%d)", p->size.x, p->size.y);
}

int obj_player_evt(ib_event* e, void* d) {
    ib_object* obj = d;
    obj_player* self = obj->d;

    switch (e->type) {
    case IB_EVT_UPDATE:
        {
            ib_graphics_point orig = obj->pos, cur = orig, bl = cur, br = cur;
            int xdir = OBJ_PLAYER_SPEED_X * (ib_input_get_key(SDL_SCANCODE_RIGHT) - ib_input_get_key(SDL_SCANCODE_LEFT)); /* sneaky logic */
            int ydir = OBJ_PLAYER_SPEED_Y * (ib_input_get_key(SDL_SCANCODE_DOWN) - ib_input_get_key(SDL_SCANCODE_UP));

            bl.x = orig.x + xdir;
            bl.y = orig.y + obj->size.y;
            br = bl;
            br.x += obj->size.x;

            if (ib_world_col_point(bl) && ib_world_col_point(br)) {
                obj->pos.x += xdir;
            } else {
                br.x -= xdir;
                bl.x -= xdir;
            }

            bl.y += ydir;
            br.y += ydir;

            if (ib_world_col_point(bl) && ib_world_col_point(br)) {
                obj->pos.y += ydir;
            }
        }
        break;
    case IB_EVT_DRAW:
        ib_graphics_set_space(IB_GRAPHICS_WORLDSPACE);
        ib_graphics_draw_texture(self->tex, obj->pos);
        break;
    }

    return 0;
}

void obj_player_destroy(ib_object* p) {
    obj_player* self = p->d;
    ib_graphics_drop_texture(self->tex);
    ib_free(p);
}
