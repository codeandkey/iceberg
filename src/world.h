#ifndef IB_WORLD
#define IB_WORLD

/*
 * world.h
 * iceberg world management functions
 *
 * tile-based worlds require interactions with many other elements of the game
 * so they have their own subsystem. iceberg supports TMX map formats and can load
 * one at a time. tile and image layer ordering is respected but objects are not.
 *
 * to influence object drawing order objects should subscribe to specific draw events
 * for which the order is known. (call order is in game.c)
 *
 * object types consist of two functions and a typename. (init, destroy functions)
 * objects must manage their own event handling and data types
 * the ib_object structure is initially populated with attributes as they appear in the map file;
 * however they do not have to be respected and objects can do whatever they want
 */


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

#define IB_WORLD_LAYER_TILE 0
#define IB_WORLD_LAYER_IMAGE 1

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

typedef struct _ib_world_node {
    int center_x, center_y, visited;
    struct _ib_world_node* children[8];
    struct _ib_world_node* path_last;
} ib_world_node;

int ib_world_init();
void ib_world_free();

int ib_world_load(const char* path);

int ib_world_max_layer();

void ib_world_update_animations(int dt);
void ib_world_render_layer(int layer);
void ib_world_render();

/* returns 0 if there is no path, otherwise returns 1 and sets dirx, diry to desired direction */
int ib_world_path_to(int from_x, int from_y, int to_x, int to_y, int* dirx, int* diry);

/* will need rewrite after transition to chunks */
int ib_world_aabb(ib_graphics_point pos, ib_graphics_point size);
int ib_world_contains(ib_graphics_point pos, ib_graphics_point size);
int ib_world_col_point(ib_graphics_point pos);

/* ib_world_create_object returns a handle to the object but in most cases you don't really need it */

void ib_world_bind_object(const char* name, ib_object_fn init, ib_object_fn destroy);
ib_object* ib_world_create_object(const char* type, const char* name, ib_hashmap* props, ib_graphics_point pos, ib_graphics_point size, float angle, int visible);
void ib_world_destroy_object(ib_object* p);
void ib_world_destroy_all();
void ib_world_object_foreach_by_type(const char* type, int (*cb)(ib_object* p, void* d), void* d);

/* helpers to deal with property values, as all of the values in the hashmap are strings */
int ib_object_get_prop_int(ib_object* p, const char* key, int def);
double ib_object_get_prop_scalar(ib_object* p, const char* key, double def);
char* ib_object_get_prop_str(ib_object* p, const char* key, char* def);

#endif
