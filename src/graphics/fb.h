#ifndef IB_FB
#define IB_FB

/*
 * fb.h
 *
 * iceberg abstraction over a framebuffer object (offscreen rendering target)
 */

#include "texture.h"

typedef struct {
    unsigned int fbo, rbo;
    ib_texture* tex;
} ib_fb;

ib_fb* ib_fb_alloc(int w, int h);
void ib_fb_free(ib_fb* p);

/* bind render target. NULL to bind back to screen */
void ib_fb_bind(ib_fb* p);

#endif
