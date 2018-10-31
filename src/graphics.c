#include "graphics.h"
#include "config.h"
#include "log.h"
#include "mem.h"
#include "hashmap.h"
#include "event.h"
#include "sprite.h"

#include <SDL2/SDL_image.h>

static struct {
    int initialized, fs;
    SDL_Window* win;
    SDL_Renderer* renderer;
    ib_hashmap* texmap;
    int space;
    int camera_x, camera_y, width, height;
} _ib_graphics_state;

ib_graphics_color ib_graphics_color_black = {0x00};
ib_graphics_color ib_graphics_color_white = {0xFF, 0xFF, 0xFF, 0x00};
ib_graphics_color ib_graphics_color_clear = {0x00, 0x00, 0x00, 0x00};

static void _ib_graphics_transform(ib_graphics_point* p);

int ib_graphics_init(void) {
    if (_ib_graphics_state.initialized) return ib_warn("already initialized");

    _ib_graphics_state.texmap = ib_hashmap_alloc(256);

    _ib_graphics_state.width = ib_config_get_int(IB_CONFIG_GRAPHICS_WIDTH, IB_GRAPHICS_DEF_WIDTH);
    _ib_graphics_state.height = ib_config_get_int(IB_CONFIG_GRAPHICS_HEIGHT, IB_GRAPHICS_DEF_HEIGHT);
    _ib_graphics_state.fs = ib_config_get_int(IB_CONFIG_GRAPHICS_FS, IB_GRAPHICS_DEF_FULLSCREEN);
    _ib_graphics_state.camera_x = _ib_graphics_state.camera_y = 0;
    _ib_graphics_state.space = IB_GRAPHICS_WORLDSPACE;

    int flags = 0;

    if (_ib_graphics_state.fs) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }


    if (SDL_CreateWindowAndRenderer(_ib_graphics_state.width,
                                    _ib_graphics_state.height,
                                    flags,
                                    &_ib_graphics_state.win,
                                    &_ib_graphics_state.renderer) < 0) {
        return ib_err("SDL window/renderer init failed");
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    _ib_graphics_state.initialized = 1;
    return ib_ok("initialized graphics");
}

void ib_graphics_free(void) {
    if (!_ib_graphics_state.initialized) {
        ib_warn("not initialized");
        return;
    }

    SDL_DestroyRenderer(_ib_graphics_state.renderer);
    SDL_DestroyWindow(_ib_graphics_state.win);

    ib_hashmap_free(_ib_graphics_state.texmap);

    _ib_graphics_state.initialized = 0;
}

void ib_graphics_clear(void) {
    ib_graphics_set_color(ib_graphics_color_clear);
    SDL_RenderClear(_ib_graphics_state.renderer);
}

void ib_graphics_swap(void) {
    SDL_RenderPresent(_ib_graphics_state.renderer);
}

void ib_graphics_set_color(ib_graphics_color c) {
    SDL_SetRenderDrawColor(_ib_graphics_state.renderer, c.r, c.g, c.b, c.a);
}

void ib_graphics_set_texture_color(ib_graphics_texture* t, ib_graphics_color c) {
    SDL_SetTextureColorMod(t->tex, c.r, c.g, c.b);
}

ib_graphics_texture* ib_graphics_get_texture(const char* path) {
    ib_graphics_texture* mapped = ib_hashmap_get(_ib_graphics_state.texmap, path);

    if (!mapped) {
        SDL_Surface* s = IMG_Load(path);

        if (!s) {
            if (!strcmp(path, IB_GRAPHICS_ERROR_TEX)) {
                ib_err("couldn't load error texture!");
                ib_event_add(IB_EVT_QUIT, NULL, 0); /* maybe unclean exit here */
                return NULL;
            }

            ib_warn("failed to load %s", path);
            ib_graphics_texture* err = ib_graphics_get_texture(IB_GRAPHICS_ERROR_TEX);
            ib_hashmap_set(_ib_graphics_state.texmap, path, err); /* not gonna work the second time */
            return err;
        }

        mapped = ib_malloc(sizeof *mapped);
        mapped->refs = 1;
        mapped->tex = SDL_CreateTextureFromSurface(_ib_graphics_state.renderer, s);
        mapped->size.x = s->w;
        mapped->size.y = s->h;
        mapped->path = strdup(path);

        SDL_FreeSurface(s);
        SDL_SetTextureBlendMode(mapped->tex, SDL_BLENDMODE_BLEND);

        ib_hashmap_set(_ib_graphics_state.texmap, path, mapped);
    } else {
        mapped->refs++;
    }

    return mapped;
}

void ib_graphics_drop_texture(ib_graphics_texture* p) {
    if (p && !--p->refs) {
        SDL_DestroyTexture(p->tex);
        ib_hashmap_drop(_ib_graphics_state.texmap, p->path);
        ib_free(p->path);
        ib_free(p);
    }
}

void ib_graphics_draw_line(ib_graphics_point a, ib_graphics_point b) {
    _ib_graphics_transform(&a);
    _ib_graphics_transform(&b);

    SDL_RenderDrawLine(_ib_graphics_state.renderer, a.x, a.y, b.x, b.y);
}

void ib_graphics_draw_outline(ib_graphics_point a, ib_graphics_point b) {
    _ib_graphics_transform(&a);

    SDL_RenderDrawLine(_ib_graphics_state.renderer, a.x, a.y, a.x, a.y + b.y);
    SDL_RenderDrawLine(_ib_graphics_state.renderer, a.x, a.y, a.x + b.x, a.y);
    SDL_RenderDrawLine(_ib_graphics_state.renderer, a.x + b.x, a.y, a.x + b.x, a.y + b.y);
    SDL_RenderDrawLine(_ib_graphics_state.renderer, a.x, a.y + b.y, a.x + b.x, a.y + b.y);
}

void ib_graphics_draw_texture_ex(ib_graphics_texture* t, ib_graphics_point pos, ib_graphics_point size, float rad, int flip_x, int flip_y, float alpha) {
    ib_graphics_point src = {0};
    ib_graphics_draw_texture_portion_ex(t, src, t->size, pos, size, rad, flip_x, flip_y, alpha);
}

void ib_graphics_draw_texture_portion_ex(ib_graphics_texture* t, ib_graphics_point src, ib_graphics_point srcsize, ib_graphics_point pos, ib_graphics_point size, float rad, int flip_x, int flip_y, float alpha) {
    /* most explicit function for rendering textures (and the only one called) */

    _ib_graphics_transform(&pos);

    SDL_Rect dest;
    dest.x = pos.x;
    dest.y = pos.y;
    dest.w = size.x;
    dest.h = size.y;

    SDL_Rect srcr;
    srcr.x = src.x;
    srcr.y = src.y;
    srcr.w = srcsize.x;
    srcr.h = srcsize.y;

    int flip = (flip_x * SDL_FLIP_HORIZONTAL) | (flip_y * SDL_FLIP_VERTICAL);

    SDL_SetTextureAlphaMod(t->tex, alpha * 255.0f);
    SDL_RenderCopyEx(_ib_graphics_state.renderer, t->tex, &srcr, &dest, rad, NULL, flip);
}

void ib_graphics_draw_sprite(ib_sprite* spr, ib_graphics_point pos) {
    /* compute the current frame coordinates and make the rectangle */
    ib_graphics_point src, srcsize;
    src.y = 0;
    src.x = spr->fw * spr->cur_frame;
    srcsize.x = spr->fw;
    srcsize.y = spr->fh;

    ib_graphics_draw_texture_portion_ex(spr->_tex, src, srcsize, pos, spr->size, spr->angle, 0, 0, spr->alpha);
}

void _ib_graphics_transform(ib_graphics_point* p) {
    if (_ib_graphics_state.space == IB_GRAPHICS_WORLDSPACE) {
        p->x -= _ib_graphics_state.camera_x;
        p->y -= _ib_graphics_state.camera_y;
    }
}

void ib_graphics_set_space(int space) {
    _ib_graphics_state.space = space;
}

void ib_graphics_set_camera(int x, int y) {
    _ib_graphics_state.camera_x = x;
    _ib_graphics_state.camera_y = y;
}

void ib_graphics_get_camera(int* x, int* y) {
    if (x) *x = _ib_graphics_state.camera_x;
    if (y) *y = _ib_graphics_state.camera_y;
}

void ib_graphics_get_size(int* x, int* y) {
    if (x) *x = _ib_graphics_state.width;
    if (y) *y = _ib_graphics_state.height;
}

void ib_graphics_set_texture_blend(ib_graphics_texture* t, int mode) {
    switch(mode) {
    case IB_GRAPHICS_BM_ALPHA:
        SDL_SetTextureBlendMode(t->tex, SDL_BLENDMODE_BLEND);
        break;
    case IB_GRAPHICS_BM_ADD:
        SDL_SetTextureBlendMode(t->tex, SDL_BLENDMODE_ADD);
        break;
    }
}
