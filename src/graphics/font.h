#ifndef IB_FONT
#define IB_FONT

#include "texture.h"

typedef struct {
    ib_texture* tex;
    unsigned int vao, vbo;
    ib_ivec2 csize;
} ib_font;

ib_font* ib_font_alloc(const char* path, int height);
void ib_font_free(ib_font* f);

#endif
