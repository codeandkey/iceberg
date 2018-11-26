#include "lighting.h"

#include "../log.h"
#include "../mem.h"

#include "texture.h"

static ib_texture* _ib_lightmap_tex, *_ib_ambient_tex;
static ib_light* _ib_light_list;
static ib_caster* _ib_caster_list;

/*
 * ib ambient light interface
 */

void ib_light_ambient_start() {
    /* start rendering ambient lightmap */
}

void ib_light_ambient_end() {
    /* perform post-processing on ambient lights */
}

void ib_light_ambient_area(ib_ivec2 pos, ib_ivec2 size, ib_color col) {
    /* define ambient light area */
}

/*
 * simple ib_light linked list functions
 */

ib_light* ib_light_alloc() {
    ib_light* out = ib_malloc(sizeof *out);
    ib_zero(out, sizeof *out);

    out->next = _ib_light_list;

    if (out->next) {
        out->next->prev = out;
    }

    _ib_light_list = out;
    return out;
}

void ib_light_free(ib_light* p) {
    if (p->prev) {
        p->prev->next = p->next;
    } else {
        _ib_light_list = p->next;
    }

    if (p->next) {
        p->next->prev = p->prev;
    }

    ib_free(p);
}

/*
 * ib_caster linked list functions
 */

ib_caster* ib_caster_alloc(int type) {
    ib_caster* out = ib_malloc(sizeof *out);
    ib_zero(out, sizeof *out);

    out->type = type;

    out->next = _ib_caster_list;

    if (_ib_caster_list) {
        _ib_caster_list->prev = out;
    }

    _ib_caster_list = out;
    return out;
}

void ib_caster_free(ib_caster* p) {
    if (p->next) {
        p->next->prev = p->prev;
    }

    if (p->prev) {
        p->prev->next = p->next;
    } else {
        _ib_caster_list = p->next;
    }

    ib_free(p);
}

/*
 * lightmap rendering functions
 */

int ib_lightmap_init() {
    return ib_ok("initialized lighting engine");
}

void ib_lightmap_free() {
}

void ib_lightmap_clear() {
}

void ib_lightmap_gen() {
}

void ib_lightmap_bind() {
}
