#include "config.h"
#include "log.h"
#include "mem.h"
#include "hashmap.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static ib_hashmap* _ib_config_map;
static void _ib_config_free_keys(const char* k, void* v);

int ib_config_init(void) {
    if (_ib_config_map) ib_warn("reloading configuration");
    ib_config_free();

    _ib_config_map = ib_hashmap_alloc(256);

    /* try and load the configuration from the file */
    FILE* in = fopen(IB_CONFIG_FILENAME, "r");

    if (!in) {
        return ib_warn("couldn't open %s for reading", IB_CONFIG_FILENAME);
    }

    char buf[IB_CONFIG_BUFLEN];
    int count = 0;

    while (fgets(buf, sizeof buf, in)) {
        char* key = strtok(buf, "= ");
        char* value = strtok(NULL, " =\n");

        if (!key || !value || !strlen(key) || !strlen(value)) continue;

        ib_ok("[%s] = [%s]", key, value);
        ib_hashmap_set(_ib_config_map, key, strdup(value));

        ++count;
    }

    return ib_ok("loaded %d keys from %s", count, IB_CONFIG_FILENAME);;
}

void ib_config_free(void) {
    if (_ib_config_map) {
        ib_hashmap_foreach(_ib_config_map, _ib_config_free_keys);
        ib_hashmap_free(_ib_config_map);

        _ib_config_map = NULL;
    }
}

int ib_config_get_int(const char* key, int def) {
    char* v = ib_hashmap_get(_ib_config_map, key);
    if (!v) return def;
    return strtol(v, NULL, 10);
}

const char* ib_config_get_str(const char* key, const char* def) {
    char* v = ib_hashmap_get(_ib_config_map, key);
    if (!v) return def;
    return v;
}

static void _ib_config_free_keys(const char* k, void* v) {
    free(v);
}
