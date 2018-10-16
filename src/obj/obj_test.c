#include "obj_test.h"
#include "../log.h"

void obj_test_init(ib_object* p) {
    ib_ok("obj_test initialized, echo key: %s", (char*) ib_hashmap_get(p->props, "echo"));
}

void obj_test_destroy(ib_object* p) {
    ib_ok("obj_test destroyed");
}
