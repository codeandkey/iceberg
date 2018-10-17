#include "config.h"
#include "log.h"
#include "mem.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct _ib_config_entry {
    char key[IB_CONFIG_BUFLEN], value[IB_CONFIG_BUFLEN];
    struct _ib_config_entry* next;
} ib_config_entry;

static ib_config_entry* _ib_config_rows;

int ib_config_init(void) {
    if (_ib_config_rows) ib_warn("reloading configuration");
    ib_config_free();

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

        ib_config_entry* n = ib_malloc(sizeof *n);
        ib_zero(n, sizeof *n);

        n->next = _ib_config_rows;
        strncpy(n->key, key, IB_CONFIG_BUFLEN - 1);
        strncpy(n->value, value, IB_CONFIG_BUFLEN - 1);

        ib_ok("[%s] = [%s]", n->key, n->value);

        _ib_config_rows = n;
        ++count;
    }

    return ib_ok("loaded %d keys from %s", count, IB_CONFIG_FILENAME);;
}

void ib_config_free(void) {
    ib_config_entry* tmp, *head = _ib_config_rows;

    while (head) {
        tmp = head->next;
        ib_free(head);
        head = tmp;
    }

    _ib_config_rows = head;
}

int ib_config_get_int(const char* key, int def) {
    ib_config_entry* cur = _ib_config_rows;

    while (cur) {
        if (!strcmp(cur->key, key)) return strtol(cur->value, NULL, 10);
        cur = cur->next;
    }

    return def;
}

const char* ib_config_get_str(const char* key, const char* def) {
    ib_config_entry* cur = _ib_config_rows;

    while (cur) {
        if (!strcmp(cur->key, key)) return cur->value;
        cur = cur->next;
    }

    return def;
}
