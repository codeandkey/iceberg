#ifndef IB_LIGHTING
#define IB_LIGHTING

/*
 * ib lighting engine
 *
 * provides an interface which allows users to grab references to a light object
 * and free it when they are done. this is similarly done with casters.
 *
 * there are different types of light considered by the engine:
 *
 *      DYNAMIC LIGHTING
 *          lights that move around and change often
 *          these cast shadows on all casters and the lightmaps are recomputed on each draw cycle
 *
 *      AMBIENT LIGHTING
 *          lights that span an area/space but do not cast shadows on moving objects
 *          the lightmap for these is additively blended with the dynamic lightmap every frame
 *
 *          this only has to be computed once and more work can go into it (blurring/etc)
 *          however storing the ambient lightmap texture requires more work and possibly splitting the world
 *          into chunks and stuff when generating
 *
 *          when computing this we don't necessarily even have to consider world casters, we can just
 *          blur the shit out of the map and let colors bleed everywhere -- might look cool
 */

/*
 * ambient lighting structures
 * these should be passed to the lighting engine and the ambient lightmap should be generated ONCE (expensive)
 */

void ib_light_ambient_start(); /* call before and after adding all the ambient lights */
void ib_light_ambient_end();

void ib_light_ambient_area(ib_ivec2 pos, ib_ivec2 size, ib_color col); /* call between start/end to define ambient light areas */

/*
 * dynamic light structure interface
 * provides lights to the rendering engine
 */

typedef struct _ib_light {
    ib_ivec2 pos; /* mess with these fields to change the behavior of the light */
    ib_color color;
    int radius;

    struct _ib_light* next, *prev; /* don't touch */
} ib_light;

ib_light* ib_light_alloc();
void ib_light_free(ib_light* p);

/*
 * caster structure interfaces
 * provides solid objects which block light
 */

#define IB_CASTER_STATIC_RECT 0

typedef struct _ib_caster {
    int type;

    struct {
        ib_ivec2 pos, size;
        int height;
    } static_rect;

    struct _ib_caster* next, *prev;
} ib_caster;

ib_caster* ib_caster_alloc(int type);
void ib_caster_free(ib_caster* p);

/*
 * lightmap rendering functions
 */

#endif
