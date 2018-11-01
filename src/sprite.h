/*
 * sprite.h
 *
 * structures to make handling textures and animations less hell
 */

#ifndef IB_SPRITE
#define IB_SPRITE

#include "graphics.h"

typedef struct _ib_sprite { /* these fields may all be manipulated except the texture don't touch that */
    ib_graphics_texture* _tex;
    float angle, alpha;
    int cur_frame, elapsed, interval, fw, fh, num_frames, playing, oneshot;
    ib_graphics_point size;
} ib_sprite;

ib_sprite* ib_sprite_alloc(const char* src);
ib_sprite* ib_sprite_alloc_animated(const char* src, int fw, int fh, int interval, int oneshot);

void ib_sprite_free(ib_sprite* p);

/* update animation frames */
void ib_sprite_update(ib_sprite* p);

/* animation control */
void ib_sprite_anim_stop(ib_sprite* p);
void ib_sprite_anim_start(ib_sprite* p);
void ib_sprite_anim_pause(ib_sprite* p);

#endif
