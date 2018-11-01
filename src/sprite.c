#include "sprite.h"
#include "log.h"
#include "mem.h"

#include "game.h" /* we need to know how much time passes in an update */

ib_sprite* ib_sprite_alloc(const char* src) {
    ib_sprite* out = ib_malloc(sizeof *out);
    ib_zero(out, sizeof *out);
    out->_tex = ib_graphics_get_texture(src);
    out->size = out->_tex->size;
    out->angle = 0.0f;
    out->alpha = 1.0f;
    out->fw = out->size.x;
    out->fh = out->size.y;
    return out;
}

ib_sprite* ib_sprite_alloc_animated(const char* src, int fw, int fh, int interval, int oneshot) {
    ib_sprite* out = ib_sprite_alloc(src);
    out->fw = fw;
    out->fh = fh;
    out->size.x = fw;
    out->size.y = fh;
    if (out->_tex->size.x % fw) {
        ib_warn("invalid sprite %s: frame width %d does not divide texture width %d", src, fw, out->_tex->size.x);
    }
    if (out->_tex->size.y != fh) {
        ib_warn("invalid sprite %s: frame height %d differs from texture height %d", src, fh, out->_tex->size.y);
    }
    out->num_frames = out->_tex->size.x / fw;
    out->oneshot = oneshot;
    out->interval = interval;
    return out;
}

void ib_sprite_free(ib_sprite* p) {
    ib_graphics_drop_texture(p->_tex);
    ib_free(p);
}

void ib_sprite_update(ib_sprite* p) {
    /* integrate animations over the elapsed time */

    if (p->playing) {
        p->elapsed += IB_GAME_MS_PER_UPDATE;

        while (p->elapsed > p->interval) {
            p->elapsed -= p->interval;

            if (++p->cur_frame >= p->num_frames) {
                p->cur_frame = 0;

                if (p->oneshot) {
                    p->playing = p->elapsed = 0;
                }
            }
        }
    }
}

void ib_sprite_anim_stop(ib_sprite* p) {
    p->playing = p->cur_frame = p->elapsed = 0;
}

void ib_sprite_anim_start(ib_sprite* p) {
    p->playing = 1;
}

void ib_sprite_anim_pause(ib_sprite* p) {
    p->playing = 0;
}
