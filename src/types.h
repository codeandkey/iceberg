#ifndef IB_TYPES
#define IB_TYPES

typedef struct {
    float x, y;
} ib_vec2;

typedef struct {
    float x, y, z;
} ib_vec3;

typedef struct {
    float x, y, z, w;
} ib_vec4;

typedef struct {
    int x, y;
} ib_ivec2;

typedef struct {
    int x, y, z;
} ib_ivec3;

typedef struct {
    int x, y, z, w;
} ib_ivec4;

typedef struct {
    float r, g, b, a;
} ib_color;

typedef struct {
    ib_vec2 pos, tc;
} ib_vert;

#endif
