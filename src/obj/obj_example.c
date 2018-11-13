/*
 * example.c
 * example object implementation
 */

/* include declarations for the object */
#include "obj_example.h"

/* include engine functions for memory, logging, graphics, and events */
#include "../event.h"
#include "../graphics/graphics.h"
#include "../log.h"
#include "../mem.h"

/* define a data structure which we store the example object data in
 * this is a user data structure -- we don't have to use it but any extra per-object data
 * for this type must be contained in the structure */

typedef struct {
    ib_texture* tex; /* store the texture for our object */
    int example_user_value; /* store 1 integer which we'll read in from a parameter */
} obj_example;

/*
 * implement the init function
 * here we allocate the obj_example structure and save it in the ib_object parent.
 * then we load any textures, resources, anything the object might need later.
 * we also subscribe to any events the object may want to respond to.
 */

void obj_example_init(ib_object* p) {
    /* allocate some memory for the extra object data, store it in p->d (object data field), and make a convienient variable we can use (self) */
    obj_example* self = p->d = ib_malloc(sizeof *self);

    /* get the value of "my_example_value" from the map file, or 12345 if it is not set */
    self->example_user_value = ib_object_get_prop_int(p, "example_user_value", 12345);

    /* log the value to the console */
    ib_ok("initialized obj_example with example_user_value=%d", self->example_user_value);

    /* load up a texture and keep it in the obj_example structure so we can use it later */
    self->tex = ib_graphics_get_texture(IB_GRAPHICS_ERROR_TEX);

    /* bind a draw event so we can respond to them */
    ib_object_subscribe(p, IB_EVT_DRAW);
}

/*
 * implement the destroy function
 * here we pretty much clean up everything we've done by initializing
 */

void obj_example_destroy(ib_object* p) {
    /* get a pointer to the extra object data */
    obj_example* self = p->d;

    /* free the texture we used */
    ib_graphics_drop_texture(self->tex);

    /* finally, free the obj_example structure we allocated for the object */
    ib_free(self);
}

/*
 * implement the event handler function
 * all of the object runtime logic is here, we change object state in response to events
 */

void obj_example_evt(ib_event* e, ib_object* obj) {
    obj_example* self = obj->d; /* retrieve obj_example* from the data field */

    /* now we can handle the events.
     * usually we subscribe multiple events to the same function, so we
     * use a switch with the event type. */

    switch(e->type) {
    case IB_EVT_DRAW:
        /*
         * on draw events..
         * draw our texture at our location
         * these fields are initially populated with the data from the map so we can just use that
         */

        ib_graphics_opt_reset();
        ib_graphics_tex_draw_ex(self->tex, obj->pos, obj->size);
        break;
    }
}
