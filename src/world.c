#include "world.h"
#include "log.h"
#include "mem.h"
#include "event.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <libgen.h>

/*
 * this is some pretty gnarly code because iceberg tries to be
 * compliant with many of the features offered in Tiled
 */

typedef struct {
    int tid;
    int duration;
} ib_world_tile_frame;

typedef struct {
    ib_world_tile_frame* frames[IB_WORLD_MAX_TILE_FRAMES];
    ib_graphics_texture* tex;
    int dt_counter; /* local tracker for elapsed time mod frame duration */
    int is_animated, frame_count, cur_frame;
} ib_world_tile;

typedef struct {
    int width, height;
    unsigned int* data;
} ib_world_tile_layer;

typedef struct {
    int visible;
    float alpha;
    ib_graphics_texture* tex;
} ib_world_image_layer;

typedef struct {
    int type, offsetx, offsety;
    ib_world_tile_layer tile;
    ib_world_image_layer img;
} ib_world_layer;

static struct {
    int initialized, twidth, theight, width, height;
    ib_world_layer* layers[IB_WORLD_MAX_LAYERS], *ground_layer;
    ib_world_tile* tiles[IB_WORLD_MAX_TID];
    ib_hashmap* obj_type_map;
    ib_object* objects;
    int subd;
    int num_layers;
} _ib_world_state;

static int _ib_world_load_layer(xmlNode* n);
static int _ib_world_load_objlayer(xmlNode* n);
static int _ib_world_load_imagelayer(xmlNode* n);
static int _ib_world_load_tileset(xmlNode* n);
static void _ib_world_free_types(const char* k, void* v);
static void _ib_world_free_props(const char* k, void* v);
static void _ib_world_unload(void);
static int _ib_world_draw_callback(ib_event* e, void* d);
static ib_graphics_texture* _ib_world_get_tex_basename(char* full_path); /* try and find a texture by the basename */

int ib_world_init() {
    if (_ib_world_state.initialized) return ib_warn("already initialized");

    LIBXML_TEST_VERSION

    ib_zero(&_ib_world_state, sizeof _ib_world_state);
    _ib_world_state.obj_type_map = ib_hashmap_alloc(256);
    _ib_world_state.initialized = 1;
    _ib_world_state.subd = ib_event_subscribe(IB_EVT_DRAW_WORLD, _ib_world_draw_callback, NULL);

    return ib_ok("initialized world");
}

void ib_world_free() {
    if (!_ib_world_state.initialized) {
        ib_warn("not initialized");
        return;
    }

    xmlCleanupParser();
    _ib_world_unload();

    ib_hashmap_foreach(_ib_world_state.obj_type_map, _ib_world_free_types);
    ib_hashmap_free(_ib_world_state.obj_type_map);

    ib_event_unsubscribe(_ib_world_state.subd);
}

int ib_world_load(const char* path) {
    xmlDoc* doc;
    xmlNode *root, *cur;

    if (!(doc = xmlReadFile(path, NULL, 0))) return ib_err("failed to parse %s", path);

    root = xmlDocGetRootElement(doc);
    if (root->type != XML_ELEMENT_NODE) return ib_err("invalid root element");
    if (strcmp((char*) root->name, "map")) return ib_err("unexpected root element type %s", root->name);

    char* prop_twidth = (char*) xmlGetProp(root, (const xmlChar*) "tilewidth");
    char* prop_theight = (char*) xmlGetProp(root, (const xmlChar*) "tileheight");
    char* prop_width = (char*) xmlGetProp(root, (const xmlChar*) "width");
    char* prop_height = (char*) xmlGetProp(root, (const xmlChar*) "height");

    _ib_world_state.twidth = strtol(prop_twidth, NULL, 10);
    _ib_world_state.theight = strtol(prop_theight, NULL, 10);
    _ib_world_state.width = strtol(prop_width, NULL, 10);
    _ib_world_state.height = strtol(prop_height, NULL, 10);

    ib_ok("loading %s..", path);
    ib_ok("tile dimensions: %s x %s", prop_twidth, prop_theight);

    xmlFree(prop_twidth);
    xmlFree(prop_theight);

    /* looks like it could be a reasonable world, unload the current one */
    _ib_world_unload();

    /* do root parsing here */
    for (cur = root->children; cur; cur = cur->next) {
        if (cur->type != XML_ELEMENT_NODE) continue;

        if (!strcmp((char*) cur->name, "layer")) {
            _ib_world_load_layer(cur);
        }

        if (!strcmp((char*) cur->name, "imagelayer")) {
            _ib_world_load_imagelayer(cur);
        }

        if (!strcmp((char*) cur->name, "objectgroup")) {
            _ib_world_load_objlayer(cur);
        }

        if (!strcmp((char*) cur->name, "tileset")) {
            _ib_world_load_tileset(cur);
        }
    }

    xmlFreeDoc(doc);
    return ib_ok("loaded %s", path);
}

int ib_world_aabb(ib_graphics_point pos, ib_graphics_point size) {
    if (ib_world_col_point(pos)) return 1;
    pos.x += size.x;
    if (ib_world_col_point(pos)) return 1;
    pos.y += size.y;
    if (ib_world_col_point(pos)) return 1;
    pos.x -= size.x;
    return ib_world_col_point(pos);
}

int ib_world_contains(ib_graphics_point pos, ib_graphics_point size) {
    if (!ib_world_col_point(pos)) return 0;
    pos.x += size.x;
    if (!ib_world_col_point(pos)) return 0;
    pos.y += size.y;
    if (!ib_world_col_point(pos)) return 0;
    pos.x -= size.x;
    return ib_world_col_point(pos);
}

int ib_world_col_point(ib_graphics_point p) {
    if (p.x < 0) return 0;
    if (p.y < 0) return 0;
    if (p.x > _ib_world_state.twidth * _ib_world_state.ground_layer->tile.width) return 0;
    if (p.y > _ib_world_state.theight * _ib_world_state.ground_layer->tile.height) return 0;

    int ptx = p.x / _ib_world_state.twidth, pty = p.y / _ib_world_state.theight;
    return _ib_world_state.ground_layer->tile.data[pty * _ib_world_state.width + ptx];
}

int _ib_world_load_objlayer(xmlNode* n) {
    /* load a layer of objects -- we don't really need to care about the layer grouping and can just add the objects */
    for (xmlNode* c = n->children; c; c = c->next) {
        if (strcmp((char*) c->name, "object")) continue;

        char* prop_name = (char*) xmlGetProp(c, (const xmlChar*) "name");
        char* prop_type = (char*) xmlGetProp(c, (const xmlChar*) "type");
        char* prop_x = (char*) xmlGetProp(c, (const xmlChar*) "x");
        char* prop_y = (char*) xmlGetProp(c, (const xmlChar*) "y");
        char* prop_width = (char*) xmlGetProp(c, (const xmlChar*) "width");
        char* prop_height = (char*) xmlGetProp(c, (const xmlChar*) "height");
        char* prop_rotation = (char*) xmlGetProp(c, (const xmlChar*) "rotation");
        char* prop_visible = (char*) xmlGetProp(c, (const xmlChar*) "visible");

        ib_hashmap* obj_props = NULL;

        for (xmlNode* p = c->children; p; p = p->next) {
            if (strcmp((char*) p->name, "properties")) continue;

            obj_props = ib_hashmap_alloc(16);

            for (xmlNode* cp = p->children; cp; cp = cp->next) {
                if (strcmp((char*) cp->name, "property")) continue;

                char* prop_key = (char*) xmlGetProp(cp, (const xmlChar*) "name");
                char* prop_value = (char*) xmlGetProp(cp, (const xmlChar*) "value");

                ib_hashmap_set(obj_props, prop_key, strdup(prop_value));

                xmlFree(prop_key);
                xmlFree(prop_value);
            }
        }

        ib_graphics_point pos, size;
        pos.x = strtol(prop_x, NULL, 10);
        pos.y = strtol(prop_y, NULL, 10);
        size.x = strtol(prop_width, NULL, 10);
        size.y = strtol(prop_height, NULL, 10);

        int visible = prop_visible ? strtol(prop_visible, NULL, 10) : 1;
        float angle = prop_rotation ? strtof(prop_rotation, NULL) : 0.0f;

        ib_object* obj = ib_world_create_object(prop_name, prop_type, obj_props, pos, size, angle, visible);

        if (!obj && obj_props) {
            ib_hashmap_foreach(obj_props, _ib_world_free_props);
            ib_hashmap_free(obj_props);
        }

        xmlFree(prop_name);
        xmlFree(prop_type);
        xmlFree(prop_x);
        xmlFree(prop_y);
        xmlFree(prop_width);
        xmlFree(prop_height);
        xmlFree(prop_rotation);
        xmlFree(prop_visible);
    }

    return 0;
}

int _ib_world_load_layer(xmlNode* n) {
    /* try and load up a world layer from an xml node */

    char* prop_id = (char*) xmlGetProp(n, (const xmlChar*) "id");
    char* prop_width = (char*) xmlGetProp(n, (const xmlChar*) "width");
    char* prop_height = (char*) xmlGetProp(n, (const xmlChar*) "height");
    char* prop_offsetx = (char*) xmlGetProp(n, (const xmlChar*) "offsetx");
    char* prop_offsety = (char*) xmlGetProp(n, (const xmlChar*) "offsety");
    char* prop_name = (char*) xmlGetProp(n, (const xmlChar*) "name");

    if (!(prop_id && prop_width && prop_height)) return ib_err("invalid layer properties");

    int id = strtol(prop_id, NULL, 10);
    int width = strtol(prop_width, NULL, 10);
    int height = strtol(prop_height, NULL, 10);
    int offsetx = prop_offsetx ? strtol(prop_offsetx, NULL, 10) : 0;
    int offsety = prop_offsety ? strtol(prop_offsety, NULL, 10) : 0;

    xmlFree(prop_id);
    xmlFree(prop_width);
    xmlFree(prop_height);
    xmlFree(prop_offsetx);
    xmlFree(prop_offsety);

    ib_world_layer* target = ib_malloc(sizeof *target);

    target->type = IB_WORLD_LAYER_TILE;
    target->tile.width = width;
    target->tile.height = height;
    target->offsetx = offsetx;
    target->offsety = offsety;

    target->tile.data = ib_malloc(width * height * sizeof *target->tile.data); /* store in row-major order starting from the top-left */

    /* look for some data */
    int loaded = 0;
    for (xmlNode* c = n->children; c; c = c->next) {
        if (!strcmp((char*) c->name, "data")) {
            /* make sure the encoding is right */
            const char* encoding = (const char*) xmlGetProp(c, (const xmlChar*) "encoding");
            if (!encoding) encoding = "none";

            if (strcmp(encoding, "csv")) {
                ib_free(target);
                return ib_err("unsupported encoding");
            }

            char* content = (char*) xmlNodeGetContent(c);

            /* parse the csv into the target data */
            int pos = 0;
            for (char* cv = strtok(content, ","); cv; cv = strtok(NULL, ",")) {
                if (pos >= width * height) {
                    ib_warn("layer data too large?");
                    break;
                }

                target->tile.data[pos++] = strtol(cv, NULL, 10);
            }

            xmlFree(content);

            loaded = 1;
            break;
        }
    }

    if (!loaded) {
        ib_free(target);
        return ib_err("failed loading layer data for %d", id);
    }

    if (!strcmp(prop_name, "ground") || !_ib_world_state.ground_layer) _ib_world_state.ground_layer = target;
    xmlFree(prop_name);

    _ib_world_state.layers[_ib_world_state.num_layers++] = target;
    return ib_ok("loaded layer %d", id);
}

int _ib_world_load_imagelayer(xmlNode* n) {
    char* prop_x = (char*) xmlGetProp(n, (const xmlChar*) "offsetx");
    char* prop_y = (char*) xmlGetProp(n, (const xmlChar*) "offsety");
    char* prop_vis = (char*) xmlGetProp(n, (const xmlChar*) "visible");
    char* prop_alpha = (char*) xmlGetProp(n, (const xmlChar*) "opacity");

    ib_world_layer* target = ib_malloc(sizeof *target);

    target->type = IB_WORLD_LAYER_IMAGE;
    target->offsetx = prop_x ? strtol(prop_x, NULL, 10) : 0;
    target->offsety = prop_y ? strtol(prop_y, NULL, 10) : 0;
    target->img.visible = prop_vis ? strtol(prop_vis, NULL, 10) : 1;
    target->img.alpha = prop_alpha ? strtof(prop_alpha, NULL) : 1.0f;
    target->img.tex = NULL;

    xmlFree(prop_x);
    xmlFree(prop_y);
    xmlFree(prop_vis);
    xmlFree(prop_alpha);

    /* look for an image in this layer */
    for (xmlNode* c = n->children; c; c = c->next) {
        if (strcmp((char*) c->name, "image")) continue;
        char* prop_src = (char*) xmlGetProp(c, (const xmlChar*) "source");
        if (!prop_src) continue;
        target->img.tex = _ib_world_get_tex_basename(prop_src);
        xmlFree(prop_src);
        break;
    }

    if (!target->img.tex) {
        target->img.tex = ib_graphics_get_texture(IB_GRAPHICS_ERROR_TEX);
    }

    _ib_world_state.layers[_ib_world_state.num_layers++] = target;
    return 0;
}

int _ib_world_load_tileset(xmlNode* n) {
    /* try and load up a tileset from an xml node */

    char* prop_firstgid = (char*) xmlGetProp(n, (const xmlChar*) "firstgid");
    if (!prop_firstgid) return ib_err("no first gid in tileset");

    int firstgid = strtol(prop_firstgid, NULL, 10);

    for (xmlNode* t = n->children; t; t = t->next) {
        if (strcmp((char*) t->name, "tile")) continue;

        char* prop_tid = (char*) xmlGetProp(t, (const xmlChar*) "id");

        if (!prop_tid) {
            ib_warn("tile missing id! skipping");
            continue;
        }

        int tid = strtol(prop_tid, NULL, 10) + firstgid;
        xmlFree(prop_tid);

        if (_ib_world_state.tiles[tid]) {
            ib_warn("duplicate tile ID %d, skipping", tid);
            continue;
        }

        ib_world_tile* target = ib_malloc(sizeof *target);
        ib_zero(target, sizeof *target);

        int fail = 1; /* if something goes wrong with loading */

        /* look for animations or images */
        for (xmlNode* c = t->children; c; c = c->next) {
            if (!strcmp((char*) c->name, "image")) {
                /* try and load up a texture */
                char* source = (char*) xmlGetProp(c, (const xmlChar*) "source");

                if (!source) {
                    ib_warn("tile image missing source");
                    continue;
                }

                target->tex = _ib_world_get_tex_basename(source);
                xmlFree(source);

                fail = 0;
                continue;
            }

            if (!strcmp((char*) c->name, "animation")) {
                if (target->is_animated) {
                    ib_warn("unexpected animation, tile already has animation");
                    continue;
                }

                /* load an animation, set tile flag as well */
                for (xmlNode* frame = c->children; frame; frame = frame->next) {
                    if (strcmp((char*) frame->name, "frame")) continue;

                    char* prop_tileid = (char*) xmlGetProp(frame, (const xmlChar*) "tileid");
                    char* prop_duration = (char*) xmlGetProp(frame, (const xmlChar*) "duration");

                    if (!(prop_tileid && prop_duration)) {
                        ib_warn("frame missing parameters");
                        continue;
                    }

                    ib_world_tile_frame* target_frame = ib_malloc(sizeof *target_frame);
                    target_frame->tid = strtol(prop_tileid, NULL, 10) + firstgid;
                    target_frame->duration = strtol(prop_duration, NULL, 10);

                    target->frames[target->frame_count++] = target_frame;

                    xmlFree(prop_tileid);
                    xmlFree(prop_duration);
                }

                if (!target->frame_count) {
                    ib_err("no frames in tile animation! ignoring");
                    continue;
                }

                fail = 0;
                target->is_animated = 1;
                continue;
            }
        }

        if (fail) {
            ib_free(target);
            ib_warn("failed loading tile data for %d", tid);
            continue;
        }

        _ib_world_state.tiles[tid] = target;
    }

    return ib_ok("loaded tileset");
}

void _ib_world_unload(void) {
    /* unload active tiles + layers, objects */
    ib_world_destroy_all();

    for (int i = 0; i < IB_WORLD_MAX_LAYERS; ++i) {
        ib_world_layer* cur = _ib_world_state.layers[i];

        if (cur) {
            if (cur->type == IB_WORLD_LAYER_TILE) {
                ib_free(cur->tile.data);
            }

            ib_free(cur);
            _ib_world_state.layers[i] = NULL;
        }
    }

    for (int i = 0; i < IB_WORLD_MAX_TID; ++i) {
        ib_world_tile* t = _ib_world_state.tiles[i];

        if (t) {
            for (int k = 0; k < IB_WORLD_MAX_TILE_FRAMES; ++k) {
                ib_world_tile_frame* f = t->frames[k];

                if (f) {
                    ib_free(t->frames[k]);
                    t->frames[k] = NULL;
                }
            }

            ib_graphics_drop_texture(t->tex);
            ib_free(t);
            _ib_world_state.tiles[i] = NULL;
        }
    }
}

void ib_world_render() {
    for (int i = 0; i < IB_WORLD_MAX_LAYERS; ++i) {
        ib_world_render_layer(i);
    }
}

int _ib_world_draw_callback(ib_event* e, void* d) {
    ib_world_render();
    return 0;
}

void ib_world_render_layer(int layer) {
    ib_world_layer* src = _ib_world_state.layers[layer];
    ib_graphics_point pos;

    ib_graphics_set_space(IB_GRAPHICS_WORLDSPACE);

    if (!src) return;

    if (src->type == IB_WORLD_LAYER_TILE) {
        for (int yi = 0; yi < src->tile.height; ++yi) {
            pos.y = yi * _ib_world_state.theight + src->offsety;

            for (int xi = 0; xi < src->tile.width; ++xi) {
                ib_world_tile* t = _ib_world_state.tiles[src->tile.data[yi * src->tile.width + xi]];
                if (!t) continue;

                pos.x = xi * _ib_world_state.twidth + src->offsetx;

                if (t->is_animated) {
                    ib_graphics_draw_texture(_ib_world_state.tiles[t->frames[t->cur_frame]->tid]->tex, pos);
                } else {
                    ib_graphics_draw_texture(t->tex, pos);
                }
            }
        }
    }

    if (src->type == IB_WORLD_LAYER_IMAGE) {
        ib_graphics_point pos;
        pos.x = src->offsetx;
        pos.y = src->offsety;
        ib_graphics_draw_texture(src->img.tex, pos);
    }
}

void ib_world_update_animations(int dt) {
    for (int i = 0; i < IB_WORLD_MAX_TID; ++i) {
        ib_world_tile* t = _ib_world_state.tiles[i];
        if (!t) continue;
        if (!t->is_animated) continue;

        t->dt_counter += dt;

        while (t->frames[t->cur_frame]->duration <= t->dt_counter) {
            t->dt_counter -= t->frames[t->cur_frame]->duration;
            t->cur_frame = (t->cur_frame + 1) % t->frame_count;
        }
    }
}

static void _ib_world_free_types(const char* k, void* v) {
    ib_object_type* t = (ib_object_type*) v;
    free(t->name);
    ib_free(v);
}

static void _ib_world_free_props(const char* k, void* v) {
    free(v); /* props are created with strdup so free() is used here */
}

void ib_world_object_foreach_by_type(const char* type, int (*cb)(ib_object* p, void* d), void* d) {
    /* iterate objects by type */
    /* return nonzero from the callback to terminate early */

    ib_object* cur = _ib_world_state.objects;

    while (cur) {
        if (!strcmp(cur->t->name, type)) {
            if (cb(cur, d)) return;
        }

        cur = cur->next;
    }
}

void ib_world_bind_object(const char* type, ib_object_fn init, ib_object_fn destroy) {
    ib_object_type* t = ib_hashmap_get(_ib_world_state.obj_type_map, type);

    if (t) {
        ib_warn("duplicate object type %s, ignoring", type);
        return;
    }

    t = ib_malloc(sizeof *t);

    t->name = strdup(type);
    t->init = init;
    t->destroy = destroy;

    ib_hashmap_set(_ib_world_state.obj_type_map, type, t);
    ib_ok("bound object type %s", type);
}

ib_object* ib_world_create_object(const char* type, const char* name, ib_hashmap* props, ib_graphics_point pos, ib_graphics_point size, float rot, int visible) {
    ib_object_type* t = ib_hashmap_get(_ib_world_state.obj_type_map, type);

    if (!t) {
        ib_warn("unknown object type %s", type);
        return NULL;
    }

    ib_object* obj = ib_malloc(sizeof *obj);
    ib_zero(obj, sizeof *obj);

    if (name) {
        obj->inst_name = strdup(name);
    }

    obj->props = props;
    obj->t = t;
    obj->pos = pos;
    obj->size = size;
    obj->angle = rot;
    obj->visible = visible;
    obj->prev = NULL;

    /* insert at head of doubly linked list */
    obj->next = _ib_world_state.objects;
    if (_ib_world_state.objects) _ib_world_state.objects->prev = obj;
    _ib_world_state.objects = obj;

    obj->t->init(obj);
    return obj;
}

void ib_world_destroy_object(ib_object* obj) {
    obj->t->destroy(obj);

    if (obj->next) {
        obj->next->prev = obj->prev;
    }

    if (obj->prev) {
        obj->prev->next = obj->next;
    } else {
        _ib_world_state.objects = obj->next;
    }

    if (obj->props) {
        ib_hashmap_foreach(obj->props, _ib_world_free_props);
        ib_hashmap_free(obj->props);
    }

    if (obj->inst_name) free(obj->inst_name);
    ib_free(obj);
}

void ib_world_destroy_all() {
    /* flush all objects out */
    ib_object* cur = _ib_world_state.objects, *tmp;

    while (cur) {
        cur->t->destroy(cur);

        if (cur->props) ib_hashmap_free(cur->props);
        if (cur->inst_name) free(cur->inst_name);

        tmp = cur->next;
        ib_free(cur);
        cur = tmp;
    }

    _ib_world_state.objects = NULL;
}

int ib_object_get_prop_int(ib_object* p, const char* key, int def) {
    if (!p || !p->props) return def;
    char* prop = ib_hashmap_get(p->props, key);
    if (!prop) return def;
    return strtol(prop, NULL, 10);
}

double ib_object_get_prop_scalar(ib_object* p, const char* key, double def) {
    if (!p || !p->props) return def;
    char* prop = ib_hashmap_get(p->props, key);
    if (!prop) return def;
    return strtod(prop, NULL);
}

char* ib_object_get_prop_str(ib_object* p, const char* key, char* def) {
    if (!p || !p->props) return def;
    char* prop = ib_hashmap_get(p->props, key);
    if (!prop) return def;
    return prop;
}

static ib_graphics_texture* _ib_world_get_tex_basename(char* fp) {
    char* bn = basename(fp);
    char* full_path = ib_malloc(strlen(bn) + strlen(IB_GRAPHICS_TEX_PREFIX) + 1);

    /* looks unsafe but the buffer has an appropriate size */
    *full_path = 0;
    strcat(full_path, IB_GRAPHICS_TEX_PREFIX);
    strcat(full_path, bn);

    ib_graphics_texture* out = ib_graphics_get_texture(full_path);
    ib_free(full_path);

    return out;
}
