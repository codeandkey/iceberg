#include "sprite.h"
#include "gl.h"
#include "graphics.h"

#include "../log.h"
#include "../mem.h"
#include "../event.h"
#include "../game.h"

static int _ib_sprite_evt(ib_event* e, void* d);

ib_sprite* ib_sprite_alloc(const char* path, int w, int h, unsigned int interval) {
    ib_sprite* out = ib_malloc(sizeof *out);
    ib_zero(out, sizeof *out);
    out->interval = interval;
    out->tex = ib_graphics_get_texture(path);

    if (out->tex->size.x % w || out->tex->size.y % h) {
        ib_warn("%s : weird frame size %dx%d doesn't divide texture %dx%d", path, w, h, out->tex->size.x, out->tex->size.y);
        ib_warn("treating this texture as a single frame. probably not gonna be pretty");
        w = out->tex->size.x;
        h = out->tex->size.y;
    }

    out->frame.x = w;
    out->frame.y = h;

    /* treat all sheets as horizontal */
    out->num_frames = out->tex->size.x / w;

    float tc_hpixel = 1.0f / out->tex->size.x; /* need this to get perfect coordinates */
    float tc_framewidth = tc_hpixel * w - tc_hpixel / 2.0f;

    /* we use vertex offsets into a VBO to render different frames. so we need to construct some weird looking vertex data */
    ib_vert* verts = ib_malloc(6 * out->num_frames * sizeof *verts);

    float tc_left = tc_hpixel / 2.0f;
    for (int i = 0; i < out->num_frames; ++i) {
        verts[i*6].pos.x = -1.0f;
        verts[i*6].pos.y = 1.0f;
        verts[i*6+1].pos.x = 1.0f;
        verts[i*6+1].pos.y = 1.0f;
        verts[i*6+2].pos.x = 1.0f;
        verts[i*6+2].pos.y = -1.0f;
        verts[i*6+3].pos.x = -1.0f;
        verts[i*6+3].pos.y = 1.0f;
        verts[i*6+4].pos.x = 1.0f;
        verts[i*6+4].pos.y = -1.0f;
        verts[i*6+5].pos.x = -1.0f;
        verts[i*6+5].pos.y = -1.0f;

        verts[i*6].tc.x = tc_left;
        verts[i*6].tc.y = 0.0f;
        verts[i*6+1].tc.x = tc_left + tc_framewidth;
        verts[i*6+1].tc.y = 0.0f;
        verts[i*6+2].tc.x = tc_left + tc_framewidth;
        verts[i*6+2].tc.y = 1.0f;
        verts[i*6+3].tc.x = tc_left;
        verts[i*6+3].tc.y = 0.0f;
        verts[i*6+4].tc.x = tc_left + tc_framewidth;
        verts[i*6+4].tc.y = 1.0f;
        verts[i*6+5].tc.x = tc_left;
        verts[i*6+5].tc.y = 1.0f;

        tc_left += tc_framewidth + tc_hpixel;
    }

    ib_glGenVertexArrays(1, &out->vao);
    ib_glBindVertexArray(out->vao);

    ib_glGenBuffers(1, &out->vbo);
    ib_glBindBuffer(GL_ARRAY_BUFFER, out->vbo);

    ib_glBufferData(GL_ARRAY_BUFFER, 6 * out->num_frames * sizeof *verts, verts, GL_STATIC_DRAW);
    ib_glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof *verts, NULL);
    ib_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof *verts, (void*) (sizeof(float) * 2));
    ib_glEnableVertexAttribArray(0);
    ib_glEnableVertexAttribArray(1);

    ib_free(verts);

    /* bind an update event */
    out->subu = ib_event_subscribe(IB_EVT_UPDATE, _ib_sprite_evt, out);

    ib_ok("initialized sprite %s, %d frames", path, out->num_frames);

    return out;
}

void ib_sprite_free(ib_sprite* s) {
    ib_graphics_drop_texture(s->tex);
    ib_glDeleteVertexArrays(1, &s->vao);
    ib_glDeleteBuffers(1, &s->vbo);
    ib_event_unsubscribe(s->subu);
    ib_free(s);
}

void ib_sprite_start(ib_sprite* s) {
    s->playing = 1;
}

void ib_sprite_pause(ib_sprite* s) {
    s->playing = 0;
}

void ib_sprite_stop(ib_sprite* s) {
    s->playing = s->cur_frame = 0;
}

void ib_sprite_oneshot(ib_sprite* s, unsigned int os) {
    s->oneshot = os;
}

int _ib_sprite_evt(ib_event* e, void* d) {
    ib_sprite* self = d;
    if (self->playing) {
        self->elapsed += IB_GAME_MS_PER_UPDATE;

        while (self->elapsed > self->interval) {
            self->elapsed -= self->interval;

            if (++self->cur_frame >= self->num_frames) {
                if (self->oneshot) {
                    self->playing = 0;
                }

                self->cur_frame = 0;
            }
        }
    }
    return 0;
}
