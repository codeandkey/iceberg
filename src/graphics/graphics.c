#include "graphics.h"
#include "gl.h"
#include "shader.h"

#include "../hashmap.h"
#include "../config.h"

/* logger utils */
#include "../log.h"

/* SDL stuff */
#include <SDL2/SDL.h>

/* linear math */
#include "../deps/linmath/linmath.h"

/*
 * static util functions
 */

/* update viewport for output dimensions */
static void _ibg_set_viewport();

/* subsystem state */
static struct {
    int initialized, width, height;
    SDL_Window* win;
    SDL_GLContext ctx;
    ib_hashmap* texmap;
    ib_shader* shd_tex, *shd_prim;
    ib_shader_opts shader_opts;
    int cur_blend, cur_space;

    mat4x4 mat_screenspace, mat_camera;
    mat4x4* mat_transform; /* pointer to current space */
    ib_ivec2 camera_pos, camera_size;

    unsigned int vao_rect, vbo_rect;
} _ibg;

/*
 * graphics function implementations
 */

/* initialize subsystem */
int ib_graphics_init() {
    if (_ibg.initialized) return ib_warn("already initialized");

    /* texture cache */
    _ibg.texmap = ib_hashmap_alloc(256);

    /* load configuration */
    _ibg.width = ib_config_get_int(IB_GRAPHICS_CFG_WIDTH, IB_GRAPHICS_WIDTH);
    _ibg.height = ib_config_get_int(IB_GRAPHICS_CFG_HEIGHT, IB_GRAPHICS_HEIGHT);

    int cfg_fs = ib_config_get_int(IB_GRAPHICS_CFG_FULLSCREEN, 0);
    int cfg_vsync = ib_config_get_int(IB_GRAPHICS_CFG_VSYNC, 0);
    int cfg_msaa = ib_config_get_int(IB_GRAPHICS_CFG_MSAA, 0);

    /* print out target video mode */
    ib_ok("initializing graphics output %dx%d msaa=%d%s%s",
          _ibg.width,
          _ibg.height,
          cfg_msaa,
          cfg_fs ? " fs" : "",
          cfg_vsync ? " vsync" : "");

    /* prepare SDL state */
    int sdl_flags = SDL_WINDOW_OPENGL | (cfg_fs * SDL_WINDOW_FULLSCREEN);

    if (cfg_msaa > 0) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, cfg_msaa);
    }

    /* request new-ish GL context */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    /* create the window */
    if (!(_ibg.win = SDL_CreateWindow("iceberg",
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      _ibg.width,
                                      _ibg.height,
                                      sdl_flags))) {
        return ib_err("failed to create SDL window");
    }

    /* init opengl context and make current */
    if (!(_ibg.ctx = SDL_GL_CreateContext(_ibg.win))) {
        return ib_err("failed to create GL context");
    }

    if (SDL_GL_MakeCurrent(_ibg.win, _ibg.ctx) < 0) {
        return ib_err("failed to set GL context");
    }

    /* load GL function pointers */
    if (ib_gl_load()) {
        return ib_err("failed to load GL function pointers");
    }

    /* initialize shaders */
    if (!(_ibg.shd_tex = ib_shader_alloc(IB_SHADER_FILE("vs_main"), IB_SHADER_FILE("fs_tex")))) {
        return ib_err("failed to init tex shader");
    }

    if (!(_ibg.shd_prim = ib_shader_alloc(IB_SHADER_FILE("vs_main"), IB_SHADER_FILE("fs_prim")))) {
        return ib_err("failed to init prim shader");
    }

    /* initialize rect VAOs */
    ib_vert verts[6] = {
        {{ -1.0f, -1.0f }, { 0.0f, 0.0f }},
        {{ 1.0f, -1.0f }, { 1.0f, 0.0f }},
        {{ 1.0f, 1.0f }, { 1.0f, 1.0f }},
        {{ -1.0f, 1.0f }, { 0.0f, 1.0f }},
    };

    ib_glGenVertexArrays(1, &_ibg.vao_rect);
    ib_glBindVertexArray(_ibg.vao_rect);

    ib_glGenBuffers(1, &_ibg.vbo_rect);
    ib_glBindBuffer(GL_ARRAY_BUFFER, _ibg.vbo_rect);

    ib_glBufferData(GL_ARRAY_BUFFER, sizeof verts, verts, GL_STATIC_DRAW);

    ib_glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ib_vert), NULL);
    ib_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ib_vert), (void*) (sizeof(ib_vec2)));
    ib_glEnableVertexAttribArray(0);
    ib_glEnableVertexAttribArray(1);

    /* prep camera stuff and transforms */
    _ibg.camera_pos.x = 0;
    _ibg.camera_pos.y = 0;
    _ibg.camera_size.x = IB_GRAPHICS_WIDTH;
    _ibg.camera_size.y = IB_GRAPHICS_HEIGHT;

    ib_graphics_set_camera(_ibg.camera_pos, _ibg.camera_size);

    mat4x4_ortho(_ibg.mat_screenspace, 0, IB_GRAPHICS_WIDTH, IB_GRAPHICS_HEIGHT, 0, 0.0, 1.0);

    /* setup viewports and other GL state */
    ib_glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    ib_glEnable(GL_BLEND);

    _ibg_set_viewport();

    ib_graphics_opt_reset();

    _ibg.initialized = 1;
    return ib_ok("initialized graphics");
}

/* free up everything */
void ib_graphics_free() {
    if (!_ibg.initialized) {
        ib_warn("not initialized");
        return;
    }
}

/* clear screen */
void ib_graphics_clear() {
    ib_glClear(GL_COLOR_BUFFER_BIT);
}

/* swap backbuffer */
void ib_graphics_swap() {
    SDL_GL_SwapWindow(_ibg.win);
}

/* get a reference to texture */
ib_texture* ib_graphics_get_texture(const char* path) {
    ib_texture* out = ib_hashmap_get(_ibg.texmap, path);

    if (out) {
        out->refs++;
    } else {
        out = ib_texture_alloc(path);

        if (!out) {
            if (!strcmp(path, IB_GRAPHICS_ERROR_TEX)) {
                ib_err("failed to load error texture!");
                return NULL;
            }

            out = ib_graphics_get_texture(IB_GRAPHICS_ERROR_TEX);
        } else {
            out->refs = 1;
        }

        ib_hashmap_set(_ibg.texmap, path, out);
    }

    return out;
}

void ib_graphics_drop_texture(ib_texture* t) {
    if (!--t->refs) {
        ib_hashmap_drop(_ibg.texmap, t->path);
        ib_texture_free(t);
    }
}

/*
 * primitive render functions
 */

void ib_graphics_prim_line(ib_ivec2 a, ib_ivec2 b) {
    /* generate a quick buffer set */
    unsigned int vao, vbo;

    ib_glGenVertexArrays(1, &vao);
    ib_glBindVertexArray(vao);

    ib_glGenBuffers(1, &vbo);
    ib_glBindBuffer(GL_ARRAY_BUFFER, vbo);

    ib_vert verts[2] = {
        { {a.x, a.y}, {0.0f, 0.0f} },
        { {b.x, b.y}, {0.0f, 0.0f} },
    };

    ib_glBufferData(GL_ARRAY_BUFFER, sizeof verts, verts, GL_STREAM_DRAW);

    ib_glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof *verts, NULL);
    ib_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof *verts, (void*) (sizeof(float) * 2));
    ib_glEnableVertexAttribArray(0);
    ib_glEnableVertexAttribArray(1);

    ib_shader_bind(_ibg.shd_prim);
    ib_shader_sync_opts(_ibg.shd_prim, &_ibg.shader_opts, 0);
    ib_shader_set_camera(_ibg.shd_prim, *_ibg.mat_transform, 0);

    ib_glDrawArrays(GL_LINES, 0, 2);

    ib_glDeleteVertexArrays(1, &vao);
    ib_glDeleteBuffers(1, &vbo);
}

void ib_graphics_prim_outline(ib_ivec2 pos, ib_ivec2 size) {
    ib_ivec2 tl = pos, tr = pos, bl = pos, br = pos;

    tr.x += size.x;
    bl.y += size.y;
    br.x += size.x;
    br.y += size.y;

    ib_graphics_prim_line(tl, tr);
    ib_graphics_prim_line(bl, br);
    ib_graphics_prim_line(tl, bl);
    ib_graphics_prim_line(tr, br);
}

/*
 * texture render functions
 */

void ib_graphics_tex_draw(ib_texture* t) {
    /* basic texture render -- VAO is centered so helper functions will need to update the pos before calling this */
    ib_glBindVertexArray(_ibg.vao_rect);
    ib_texture_bind(t);
    ib_shader_bind(_ibg.shd_tex);
    ib_shader_sync_opts(_ibg.shd_tex, &_ibg.shader_opts, 0);
    ib_shader_set_camera(_ibg.shd_tex, *_ibg.mat_transform, 0);
    ib_glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void ib_graphics_tex_draw_ex(ib_texture* t, ib_ivec2 pos, ib_ivec2 size) {
    ib_graphics_opt_scale_tex(t, size);
    ib_graphics_opt_pos_tex(t, pos);
    ib_graphics_tex_draw(t);
}

void ib_graphics_tex_draw_sprite(ib_sprite* s, ib_ivec2 pos) {
    /* offset position to match tex corner */
    pos.x += s->frame.x / 2;
    pos.y += s->frame.y / 2;
    ib_graphics_opt_pos(pos);

    ib_vec2 scale;
    scale.x = s->frame.x / 2.0f;
    scale.y = s->frame.y / 2.0f;
    ib_graphics_opt_scale(scale);

    ib_texture_bind(s->tex);

    /* bind VAOs and render up */
    ib_glBindVertexArray(s->vao);
    ib_shader_bind(_ibg.shd_tex);
    ib_shader_sync_opts(_ibg.shd_tex, &_ibg.shader_opts, 0);
    ib_shader_set_camera(_ibg.shd_tex, *_ibg.mat_transform, 0);
    ib_glDrawArrays(GL_TRIANGLE_FAN, 4 * s->cur_frame, 4);
}

/*
 * camera manipulation
 */

void ib_graphics_set_camera(ib_ivec2 pos, ib_ivec2 size) {
    _ibg.camera_pos = pos;
    _ibg.camera_size = size;

    mat4x4_ortho(_ibg.mat_camera, pos.x, pos.x + size.x, pos.y + size.y, pos.y, 0.0, 1.0);
}

void ib_graphics_get_camera(ib_ivec2* pos, ib_ivec2* size) {
    if (pos) *pos = _ibg.camera_pos;
    if (size) *size = _ibg.camera_size;
}

/*
 * opt setters
 */

void ib_graphics_opt_reset() {
    ib_color col = {1, 1, 1, 1};
    ib_vec2 scale = {1, 1};
    ib_ivec2 pos = {0, 0};

    ib_graphics_opt_color(col);
    ib_graphics_opt_scale(scale);
    ib_graphics_opt_pos(pos);
    ib_graphics_opt_rot(0.0f);
    ib_graphics_opt_blend(IB_GRAPHICS_BM_ALPHA);
    ib_graphics_opt_space(IB_GRAPHICS_WORLDSPACE);
}

void ib_graphics_opt_color(ib_color c) {
    _ibg.shader_opts.col = c;
}

void ib_graphics_opt_pos(ib_ivec2 pos) {
    _ibg.shader_opts.pos.x = pos.x;
    _ibg.shader_opts.pos.y = pos.y;
}

void ib_graphics_opt_scale(ib_vec2 scale) {
    _ibg.shader_opts.scale = scale;
}

void ib_graphics_opt_rot(float rot) {
    _ibg.shader_opts.rot = rot;
}

void ib_graphics_opt_blend(int mode) {
    if (mode == _ibg.cur_blend) return;
    _ibg.cur_blend = mode;

    switch (mode) {
    case IB_GRAPHICS_BM_ALPHA:
        ib_glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case IB_GRAPHICS_BM_ADD:
        ib_glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
        break;
    }
}

void ib_graphics_opt_space(int mode) {
    switch (mode) {
    case IB_GRAPHICS_WORLDSPACE:
        _ibg.mat_transform = &_ibg.mat_camera;
        break;
    case IB_GRAPHICS_SCREENSPACE:
        _ibg.mat_transform = &_ibg.mat_screenspace;
        break;
    }
}

void ib_graphics_opt_scale_tex(ib_texture* t, ib_ivec2 size) {
    /* compute appropriate scale factors for a texture */
    ib_vec2 sc;

    sc.x = (float) t->size.x / 2.0f;
    sc.y = (float) t->size.y / 2.0f;

    ib_graphics_opt_scale(sc);
}

void ib_graphics_opt_pos_tex(ib_texture* t, ib_ivec2 pos) {
    /* compute an additonal pos with the texture size offsets */
    pos.x += t->size.x / 2;
    pos.y += t->size.y / 2;
    ib_graphics_opt_pos(pos);
}

void ib_graphics_opt_alpha(float a) {
    ib_color c = { 1, 1, 1, 1 };
    c.a = a;
    ib_graphics_opt_color(c);
}

void _ibg_set_viewport() {
    /* helper to keep the game aspect ratio but stretch the viewport as much as we can */
    float scale = fmin((float) _ibg.width / (float) IB_GRAPHICS_WIDTH, (float) _ibg.height / (float) IB_GRAPHICS_HEIGHT);

    int outw = (float) IB_GRAPHICS_WIDTH * scale;
    int outh = (float) IB_GRAPHICS_HEIGHT * scale;

    ib_glViewport((_ibg.width - outw) / 2, (_ibg.height - outh) / 2, outw, outh);
}
