#ifndef IB_SHADER
#define IB_SHADER

#include "../types.h"

#include "../deps/linmath/linmath.h"

#define IB_SHADER_FILE(x) "res/shaders/" x ".glsl"

typedef struct {
    ib_vec2 scale, pos;
    ib_color col;
    float rot;
} ib_shader_opts;

typedef struct {
    unsigned int vs, fs, prg;
    int p_col, p_pos, p_scale, p_rot, p_camera;
    mat4x4 camera;
    ib_shader_opts opts;
} ib_shader;

ib_shader* ib_shader_alloc(const char* vs, const char* fs);
void ib_shader_free(ib_shader* p);

/* make sure to bind before setting opts/mats */
void ib_shader_bind(ib_shader* p);

/* upload camera matrix. should be bound */
void ib_shader_set_camera(ib_shader* p, mat4x4 cam, int force);

/* synchronize options to uniforms */
void ib_shader_sync_opts(ib_shader* p, ib_shader_opts* opts, int force);

#endif
