#ifndef IB_TEXTURE
#define IB_TEXTURE

#include "../types.h"

#define IB_TEXTURE_PREFIX "res/img/"
#define IB_TEXTURE_FILE(x) IB_TEXTURE_PREFIX x ".png"

typedef struct {
    ib_ivec2 size;
    unsigned int tex, refs;
    char* path;
} ib_texture;

ib_texture* ib_texture_alloc(const char* path);
void ib_texture_free(ib_texture* t);

void ib_texture_bind(ib_texture* t);

#endif
