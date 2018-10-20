#include "obj_player.h"

#include "../graphics.h"
#include "../event.h"
#include "../mem.h"
#include "../log.h"
#include "../input.h"

#define OBJ_PLAYER_TEXTURE IB_GRAPHICS_TEXFILE("player")
#define OBJ_PLAYER_SPEED_Y 2 /* !! 3 dimensions! */
#define OBJ_PLAYER_SPEED_X 3
#define OBJ_PLAYER_BASE_HEIGHT 12 /* height of the collision box for world movement, base at the bottom of the player sprite */
#define OBJ_PLAYER_CAMERA_FACTOR 16.0f /* increase for slower camera movement. 1.0f <=> camera will always be on player */

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

    int cx, cy;
    ib_graphics_get_camera(&cx, &cy);

    switch (e->type) {
    case IB_EVT_UPDATE:
        {
            ib_graphics_point base_pos = obj->pos;
            int xdir = OBJ_PLAYER_SPEED_X * (ib_input_get_key(SDL_SCANCODE_RIGHT) - ib_input_get_key(SDL_SCANCODE_LEFT)); /* sneaky logic */
            int ydir = OBJ_PLAYER_SPEED_Y * (ib_input_get_key(SDL_SCANCODE_DOWN) - ib_input_get_key(SDL_SCANCODE_UP));

            ib_graphics_point base_size;
            base_size.x = obj->size.x;
            base_size.y = OBJ_PLAYER_BASE_HEIGHT;

            base_pos.x += xdir;
            base_pos.y += obj->size.y - OBJ_PLAYER_BASE_HEIGHT;

            if (ib_world_contains(base_pos, base_size)) {
                obj->pos.x += xdir;
            } else {
                base_pos.x -= xdir;
            }

            base_pos.y += ydir;

            if (ib_world_contains(base_pos, base_size)) {
                obj->pos.y += ydir;
            }

            /* update camera position */

            float target_cx = obj->pos.x + obj->size.x / 2 - IB_GRAPHICS_WIDTH / 2;
            float target_cy = obj->pos.y + obj->size.y / 2 - IB_GRAPHICS_HEIGHT / 2;

            float new_cx = (float) cx + (target_cx - (float) cx) / OBJ_PLAYER_CAMERA_FACTOR;
            float new_cy = (float) cy + (target_cy - (float) cy) / OBJ_PLAYER_CAMERA_FACTOR;

            ib_graphics_set_camera((int) new_cx, (int) new_cy);
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
