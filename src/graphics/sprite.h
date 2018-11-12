#ifndef IB_SPRITE
#define IB_SPRITE

#include "../types.h"
#include "texture.h"

typedef struct {
    unsigned int vao, vbo, playing, cur_frame, num_frames, interval, oneshot, elapsed;
    int subu;
    ib_ivec2 frame;
    ib_texture* tex;
} ib_sprite;

ib_sprite* ib_sprite_alloc(const char* path, int w, int h, unsigned int interval);
void ib_sprite_free(ib_sprite* s);

void ib_sprite_start(ib_sprite* s);
void ib_sprite_pause(ib_sprite* s);
void ib_sprite_stop(ib_sprite* s);
void ib_sprite_oneshot(ib_sprite* s, unsigned int os);

#endif
