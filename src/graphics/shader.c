#include "shader.h"
#include "gl.h"

#include "../mem.h"
#include "../log.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int _ib_shader_init_shader(GLenum type, const char* path, unsigned int* out);
static int _ib_shader_init_prg(unsigned int vs, unsigned int fs, unsigned int* out);

ib_shader* ib_shader_alloc(const char* vs, const char* fs) {
    ib_shader* out = ib_malloc(sizeof *out);

    if (_ib_shader_init_shader(GL_VERTEX_SHADER, vs, &out->vs)) {
        ib_err("VS compile failed");
        return NULL;
    }

    if (_ib_shader_init_shader(GL_FRAGMENT_SHADER, fs, &out->fs)) {
        ib_err("FS compile failed");
        return NULL;
    }

    if (_ib_shader_init_prg(out->vs, out->fs, &out->prg)) {
        ib_err("program link failed");
        return NULL;
    }

    /* locate opt uniforms in shader */
    out->p_col = ib_glGetUniformLocation(out->prg, "ib_color");
    out->p_pos = ib_glGetUniformLocation(out->prg, "ib_pos");
    out->p_scale = ib_glGetUniformLocation(out->prg, "ib_scale");
    out->p_rot = ib_glGetUniformLocation(out->prg, "ib_rot");
    out->p_camera = ib_glGetUniformLocation(out->prg, "ib_camera");

    /* no need to store the texture sampler uniform */
    ib_glUniform1i(ib_glGetUniformLocation(out->prg, "ib_texture"), 0);

    /* initialize sensible defaults */
    memset(&out->opts, 0, sizeof out->opts);
    ib_color col = {1, 1, 1, 1};
    ib_vec2 scale = {1, 1};

    out->opts.col = col;
    out->opts.scale = scale;

    ib_shader_bind(out);
    ib_shader_sync_opts(out, &out->opts, 1);

    mat4x4 ident;
    mat4x4_identity(ident);

    ib_shader_set_camera(out, ident, 1);

    ib_ok("initialized shader object %s, %s", vs, fs);
    return out;
}

void ib_shader_free(ib_shader* p) {
    ib_glDeleteProgram(p->prg);
    ib_glDeleteShader(p->vs);
    ib_glDeleteShader(p->fs);
    ib_free(p);
}

void ib_shader_bind(ib_shader* p) {
    ib_glUseProgram(p->prg);
}

void ib_shader_sync_opts(ib_shader* p, ib_shader_opts* opts, int force) {
    if (force || memcmp(&opts->col, &p->opts.col, sizeof opts->col)) {
        ib_glUniform4fv(p->p_col, 1, (const float*) &opts->col);
    }

    if (force || memcmp(&opts->scale, &p->opts.scale, sizeof opts->scale)) {
        ib_glUniform2fv(p->p_scale, 1, (const float*) &opts->scale);
    }

    if (force || memcmp(&opts->pos, &p->opts.pos, sizeof opts->pos)) {
        ib_glUniform2fv(p->p_pos, 1, (const float*) &opts->pos);
    }

    if (force || (opts->rot != p->opts.rot)) {
        ib_glUniform1f(p->p_rot, opts->rot);
    }

    memcpy(&p->opts, opts, sizeof *opts);
}

void ib_shader_set_camera(ib_shader* p, mat4x4 camera, int force) {
    if (force || memcmp(*p->camera, *camera, sizeof(float) * 16)) {
        ib_glUniformMatrix4fv(p->p_camera, 1, GL_FALSE, (float*) *camera);
        memcpy(*p->camera, *camera, sizeof(float) * 16);
    }
}

int _ib_shader_init_shader(GLenum type, const char* path, unsigned int* out) {
    /* read path into a buffer */

    FILE* f = fopen(path, "rb");
    if (!f) return ib_err("failed to open %s for reading", path);

    int flen;
    fseek(f, 0, SEEK_END);
    flen = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* fbuf = ib_malloc(flen);

    if (fread(fbuf, 1, flen, f) < flen) {
        fclose(f);
        return ib_err("%s: read fail", path);
    }

    fclose(f);

    /* compile shader program */

    *out = ib_glCreateShader(type);
    ib_glShaderSource(*out, 1, (const GLchar**)  &fbuf, &flen);
    ib_glCompileShader(*out);

    /* check status */
    int res;
    ib_glGetShaderiv(*out, GL_COMPILE_STATUS, &res);

    if (!res) {
        int llen;
        ib_glGetShaderiv(*out, GL_INFO_LOG_LENGTH, &llen);
        char* lbuf = ib_malloc(llen);
        ib_zero(lbuf, llen);
        ib_glShaderGetInfoLog(*out, llen, NULL, lbuf);
        ib_err("%s compile failed: %s", path, lbuf);
        ib_free(lbuf);
        return -1;
    }

    return ib_ok("compiled %s", path);
}

int _ib_shader_init_prg(unsigned int vs, unsigned int fs, unsigned int* out) {
    *out = ib_glCreateProgram();

    /* associate shaders */
    ib_glAttachShader(*out, vs);
    ib_glAttachShader(*out, fs);

    /* link program */
    ib_glLinkProgram(*out);

    /* check status */
    int res;
    ib_glGetProgramiv(*out, GL_LINK_STATUS, &res);

    if (!res) {
        char log[1024] = {0};
        ib_glProgramGetInfoLog(*out, sizeof log, NULL, log);
        return ib_err("program link failed: \n%s\n", log);
    }

    return 0;
}
