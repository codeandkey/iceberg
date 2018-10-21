#ifndef IB_GRAPHICS
#define IB_GRAPHICS

#define IB_GRAPHICS_WIDTH 800
#define IB_GRAPHICS_HEIGHT 600

#define IB_GRAPHICS_DEF_FULLSCREEN 0
#define IB_GRAPHICS_DEF_VSYNC 0

#define IB_GRAPHICS_SCREENSPACE 0
#define IB_GRAPHICS_WORLDSPACE 1

#define IB_GRAPHICS_BM_ALPHA 0
#define IB_GRAPHICS_BM_ADD 1

#define IB_GRAPHICS_TEX_PREFIX "res/img/"
#define IB_GRAPHICS_TEX_SUFFIX ".png"
#define IB_GRAPHICS_TEXFILE(x) IB_GRAPHICS_TEX_PREFIX x IB_GRAPHICS_TEX_SUFFIX
#define IB_GRAPHICS_ERROR_TEX IB_GRAPHICS_TEXFILE("error")

#include <SDL2/SDL.h>

typedef struct {
    uint8_t r, g, b, a;
} ib_graphics_color;

typedef struct {
    int x, y;
} ib_graphics_point;

typedef struct {
    char* path;
    int refs;
    ib_graphics_point size;
    SDL_Texture* tex;
} ib_graphics_texture;

extern ib_graphics_color ib_graphics_color_black, ib_graphics_color_white, ib_graphics_color_clear;

int ib_graphics_init();
void ib_graphics_free();

void ib_graphics_clear();
void ib_graphics_swap();

ib_graphics_texture* ib_graphics_get_texture(const char* path);
void ib_graphics_drop_texture(ib_graphics_texture* p); /* reference counted */

void ib_graphics_set_camera(int x, int y);
void ib_graphics_get_camera(int* x, int* y);
void ib_graphics_get_size(int* w, int* h);
void ib_graphics_set_color(ib_graphics_color c);
void ib_graphics_set_texture_color(ib_graphics_texture* t, ib_graphics_color c);
void ib_graphics_set_texture_blend(ib_graphics_texture* t, int mode);

void ib_graphics_set_space(int space); /* define coordinate transformation -- screenspace or worldspace */

void ib_graphics_draw_line(ib_graphics_point a, ib_graphics_point b);
void ib_graphics_draw_texture_ex(ib_graphics_texture* t, ib_graphics_point pos, ib_graphics_point size, float rad, int flip_h, int flip_v, float alpha);

/* helper macros to make things a little more readable */

#define ib_graphics_draw_texture(t, pos) ib_graphics_draw_texture_ex(t, pos, t->size, 0.0f, 0, 0, 1.0f)
#define ib_graphics_draw_texture_size(t, pos, size) ib_graphics_draw_texture_ex(t, pos, size, 0.0f, 0, 0, 1.0f)
#define ib_graphics_draw_texture_rot(t, pos, rot) ib_graphics_draw_texture_ex(t, pos, t->size, rot, 0, 0, 1.0f)
#define ib_graphics_draw_texture_alpha(t, pos, alpha) ib_graphics_draw_texture_ex(t, pos, t->size, 0.0f, 0, 0, alpha)

#endif
