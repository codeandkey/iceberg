#include "font.h"
#include "gl.h"
#include "graphics.h"

#include "../mem.h"
#include "../log.h"

/* basic ASCII font sheet */

ib_font* ib_font_alloc(const char* path, int height) {
    ib_font* out = ib_malloc(sizeof *out);
    out->tex = ib_graphics_get_texture(path);

    int internal_glyph_width = out->tex->size.x / 256;

    out->csize.y = height;
    out->csize.x = (float) internal_glyph_width * ((float) height / (float) out->tex->size.y);

    /*
     * generate a big VAO for switching glyph texture coordinates
     */

    ib_vert verts[1536]; /* mr stack i don't feel so good */

    for (int i = 0; i < 256; ++i) {
        /* first tri - lower right */
        verts[i*6].pos.x = -1.0f;
        verts[i*6].pos.y = 1.0f;
        verts[i*6].tc.x = i / 256.0f;
        verts[i*6].tc.y = 0.0f;

        verts[i*6+1].pos.x = 1.0f;
        verts[i*6+1].pos.y = 1.0f;
        verts[i*6+1].tc.x = (i + 1) / 256.0f;
        verts[i*6+1].tc.y = 0.0f;

        verts[i*6+2].pos.x = 1.0f;
        verts[i*6+2].pos.y = -1.0f;
        verts[i*6+2].tc.x = (i + 1) / 256.0f;
        verts[i*6+2].tc.y = 1.0f;

        /* second tri - top left */
        verts[i*6+3].pos.x = -1.0f;
        verts[i*6+3].pos.y = 1.0f;
        verts[i*6+3].tc.x = i / 256.0f;
        verts[i*6+3].tc.y = 0.0f;

        verts[i*6+4].pos.x = 1.0f;
        verts[i*6+4].pos.y = -1.0f;
        verts[i*6+4].tc.x = (i + 1) / 256.0f;
        verts[i*6+4].tc.y = 1.0f;

        verts[i*6+5].pos.x = -1.0f;
        verts[i*6+5].pos.y = -1.0f;
        verts[i*6+5].tc.x = i / 256.0f;
        verts[i*6+5].tc.y = 1.0f;
    }

    ib_glGenVertexArrays(1, &out->vao);
    ib_glBindVertexArray(out->vao);

    ib_glGenBuffers(1, &out->vbo);
    ib_glBindBuffer(GL_ARRAY_BUFFER, out->vbo);

    ib_glBufferData(GL_ARRAY_BUFFER, sizeof verts, verts, GL_STATIC_DRAW);

    ib_glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof *verts, NULL);
    ib_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof *verts, (void*) (sizeof(float) * 2));

    ib_glEnableVertexAttribArray(0);
    ib_glEnableVertexAttribArray(1);

    ib_ok("loaded font glyphs from %s", path);
    return out;
}

void ib_font_free(ib_font* p) {
    ib_glDeleteBuffers(1, &p->vbo);
    ib_glDeleteVertexArrays(1, &p->vao);
    ib_graphics_drop_texture(p->tex);
    ib_free(p);
}
