#ifndef IB_GRAPHICS
#define IB_GRAPHICS

/*
 * iceberg graphics subsystem
 */

#include "../types.h"
#include "texture.h"
#include "sprite.h"

/* consts */

#define IB_GRAPHICS_WIDTH 800
#define IB_GRAPHICS_HEIGHT 600

#define IB_GRAPHICS_CFG_FULLSCREEN "graphics.fullscreen"
#define IB_GRAPHICS_CFG_VSYNC "graphics.vsync"
#define IB_GRAPHICS_CFG_MSAA "graphics.msaa"
#define IB_GRAPHICS_CFG_WIDTH "graphics.width"
#define IB_GRAPHICS_CFG_HEIGHT "graphics.height"

#define IB_GRAPHICS_ERROR_TEX IB_TEXTURE_FILE("error")

#define IB_GRAPHICS_BM_ALPHA 0
#define IB_GRAPHICS_BM_ADD 1

#define IB_GRAPHICS_WORLDSPACE 0
#define IB_GRAPHICS_SCREENSPACE 1


/* initialize the subsystem */
int ib_graphics_init();

/* free the subsystem */
void ib_graphics_free();

/* clear/swap buffers */
void ib_graphics_clear();
void ib_graphics_swap();

/* camera manipulation */
void ib_graphics_set_camera(ib_ivec2 pos, ib_ivec2 size);
void ib_graphics_get_camera(ib_ivec2* pos, ib_ivec2* size);

/* texture manipulation */
ib_texture* ib_graphics_get_texture(const char* path);
void ib_graphics_drop_texture(ib_texture* t);

/* prim rendering */
void ib_graphics_prim_line(ib_ivec2 a, ib_ivec2 b);
void ib_graphics_prim_outline(ib_ivec2 pos, ib_ivec2 size);

/* texture rendering */
void ib_graphics_tex_draw(ib_texture* t);
void ib_graphics_tex_draw_ex(ib_texture* t, ib_ivec2 pos, ib_ivec2 size);

/* sprite rendering */
void ib_graphics_tex_draw_sprite(ib_sprite* s, ib_ivec2 pos);

/*
 * opt settings
 *
 * the new graphics system uses a large state machine with configurable render options
 * use these to configure render options such as angles, colors, blendmodes, spaces, etc..
 *
 * before ANY rendering starts you should call opt_reset() to reset everything to these defaults:
 *
 *   pos [ivec2]  (0, 0)
 *   rot [float]  0
 *   scale [vec2] (1, 1)
 *   color [vec4] (1, 1, 1, 1)
 *   blend [mode] IB_GRAPHICS_BM_ALPHA
 *   space [mode] IB_GRAPHICS_WORLDSPACE
 */

void ib_graphics_opt_reset();

void ib_graphics_opt_color(ib_color col);
void ib_graphics_opt_scale(ib_vec2 scale);
void ib_graphics_opt_rot(float rot);
void ib_graphics_opt_pos(ib_ivec2 pos);
void ib_graphics_opt_blend(int mode);
void ib_graphics_opt_space(int space);

/* opt shortcut functions */
void ib_graphics_opt_alpha(float a); /* opt_color() but just alpha */

/* auto compute pos and scale for a rect */
void ib_graphics_opt_rect(ib_ivec2 pos, ib_ivec2 size);

#endif
