/*
 * example.h
 * example object template
 *
 * the implementation of the object is located in example.c
 * the object type must be bound to the typename "example" in obj.c
 */

/* include guard -- should be OBJ_xxx */
#ifndef OBJ_EXAMPLE
#define OBJ_EXAMPLE

/* include world.h for the ib_object type */
#include "../world.h"

/* function declarations for init() and destroy() */
void obj_example_init(ib_object* p);
void obj_example_destroy(ib_object* p);

/* end include guard*/
#endif
