#ifndef IB_WORLD
#define IB_WORLD

#include "hashmap.h"
#include "graphics.h"

/* in an ideal world we would have the world as an object itself
 * however, almost every object in the game will interact with the world somehow
 * we add engine support for easily loading and rendering maps from Tiled.
 *
 * we'll support all the cool stuff like animations and collision maps */

#define IB_WORLD_WORLDFILE(x) "res/world/" x ".tmx"
#define IB_WORLD_DEFAULT IB_WORLD_WORLDFILE("default")

#define IB_WORLD_MAX_TID 256
#define IB_WORLD_MAX_LAYERS 8
#define IB_WORLD_MAX_TILE_FRAMES 8

struct _ib_object;

typedef void (*ib_object_fn)(struct _ib_object* p);

typedef struct {
    char* name; /* copied */
    ib_object_fn init, destroy;
} ib_object_type;

typedef struct _ib_object {
    char* inst_name; /* don't mutate */
    ib_hashmap* props; /* don't mutate */
    ib_object_type* t; /* don't mutate */
    struct _ib_object* next, *prev; /* good lord don't mutate */
    void* d; /* go for it */
    ib_graphics_point pos, size; /* object don't really have to respect these at all, feel free to mutate */
    float angle; /* just properties loaded from the map */
    int visible;
} ib_object;

int ib_world_init();
void ib_world_free();

int ib_world_load(const char* path);

int ib_world_max_layer();

void ib_world_update_animations(int dt);
void ib_world_render_layer(int layer);
void ib_world_render();

/* will need rewrite after transition to chunks */
int ib_world_aabb(ib_graphics_point pos, ib_graphics_point size);
int ib_world_contains(ib_graphics_point pos, ib_graphics_point size);
int ib_world_col_point(ib_graphics_point pos);

/* ib_world_create_object returns a handle to the object but in most cases you don't really need it */

void ib_world_bind_object(const char* name, ib_object_fn init, ib_object_fn destroy);
ib_object* ib_world_create_object(const char* type, const char* name, ib_hashmap* props, ib_graphics_point pos, ib_graphics_point size, float angle, int visible);
void ib_world_destroy_object(ib_object* p);
void ib_world_destroy_all();

/* helpers to deal with property values, as all of the values in the hashmap are strings */
int ib_object_get_prop_int(ib_object* p, const char* key, int def);
double ib_object_get_prop_scalar(ib_object* p, const char* key, double def);
char* ib_object_get_prop_str(ib_object* p, const char* key, char* def);

#endif
