#include "texture.h"
#include "gl.h"

#include "../mem.h"
#include "../log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../deps/stb_image/stb_image.h"

ib_texture* ib_texture_alloc(const char* path) {
    /* stb_image doesn't load in the right orientation for GL normally */
    stbi_set_flip_vertically_on_load(1);

    int w, h;
    unsigned char* stbd = stbi_load(path, &w, &h, NULL, 4);

    if (!stbd) {
        ib_err("failed to load texture %s", path);
        return NULL;
    }

    ib_texture* out = ib_malloc(sizeof *out);
    out->size.x = w;
    out->size.y = h;
    out->refs = 0;
    out->path = strdup(path);

    ib_glGenTextures(1, &out->tex);
    ib_glBindTexture(GL_TEXTURE_2D, out->tex);
    ib_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, stbd);

    stbi_image_free(stbd);

    ib_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ib_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return out;
}

void ib_texture_free(ib_texture* t) {
    ib_glDeleteTextures(1, &t->tex);
    free(t->path);
    ib_free(t);
}

void ib_texture_bind(ib_texture* t) {
    ib_glBindTexture(GL_TEXTURE_2D, t->tex);
}
