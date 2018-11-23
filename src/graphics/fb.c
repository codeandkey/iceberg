#include "fb.h"
#include "gl.h"

#include "../log.h"
#include "../mem.h"

ib_fb* ib_fb_alloc(int w, int h) {
    ib_fb* out = ib_malloc(sizeof *out);

    ib_glGetError();

    ib_glGenFramebuffers(1, &out->fbo);
    ib_glBindFramebuffer(GL_FRAMEBUFFER, out->fbo);

    out->tex = ib_malloc(sizeof *out->tex); /* make a fake texture object */

    out->tex->path = strdup("(framebuffer)");
    out->tex->refs = 1;

    ib_glGenTextures(1, &out->tex->tex);
    ib_glBindTexture(GL_TEXTURE_2D, out->tex->tex);
    ib_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    out->tex->size.x = w;
    out->tex->size.y = h;

    ib_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ib_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    ib_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    ib_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    ib_glGenRenderbuffers(1, &out->rbo);
    ib_glBindRenderbuffer(GL_RENDERBUFFER, out->rbo);

    ib_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    ib_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, out->rbo);

    if (ib_glGetError() != GL_NO_ERROR) {
        ib_warn("GL error occurred initializing renderbuffer");
    }

    ib_glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, out->tex->tex, 0);

    GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
    ib_glDrawBuffers(1, buffers);

    if (ib_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ib_err("failed to initialize framebuffer object");
    }

    ib_ok("initialized framebuffer object %dx%d", w, h);
    return out;
}

void ib_fb_free(ib_fb* p) {
    ib_glDeleteFramebuffers(1, &p->fbo);
    ib_glDeleteRenderbuffers(1, &p->rbo);
    ib_texture_free(p->tex);
}

void ib_fb_bind(ib_fb* p) {
    if (p) {
        ib_glBindFramebuffer(GL_FRAMEBUFFER, p->fbo);
    } else {
        ib_glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
