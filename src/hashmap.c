#include <string.h>
#include <stdint.h>

#include "hashmap.h"
#include "mem.h"
#include "log.h"
#include "deps/SuperFastHash/SuperFastHash.h"

ib_hashmap* ib_hashmap_alloc(int table_size) {
    ib_hashmap* out = ib_malloc(sizeof *out);
    out->buf = ib_malloc(table_size * sizeof out->buf[0]);
    ib_zero(out->buf, table_size * sizeof out->buf[0]);
    out->len = table_size;
    return out;
}

void ib_hashmap_free(ib_hashmap* p) {
    ib_hashmap_entry* tmp;

    for (int i = 0; i < p->len; ++i) {
        while (p->buf[i]) {
            tmp = p->buf[i]->next;
            ib_free(p->buf[i]);
            p->buf[i] = tmp;
        }
    }

    ib_free(p);
}

void* ib_hashmap_get(ib_hashmap* p, const char* key) {
    ib_hashmap_entry* cur = p->buf[SuperFastHash(key, strlen(key)) % p->len];

    while (cur) {
        if (!strcmp(key, cur->key)) return cur->value;
        cur = cur->next;
    }

    return NULL;
}

void ib_hashmap_set(ib_hashmap* p, const char* key, void* value) {
    ib_hashmap_entry** root = p->buf + (SuperFastHash(key, strlen(key)) % p->len), *cur = *root;

    while (cur) {
        if (!strcmp(key, cur->key)) {
            cur->value = value;
            return;
        }

        cur = cur->next;
    }

    ib_hashmap_entry* n = ib_malloc(sizeof *n);
    n->key = strdup(key);
    n->value = value;
    n->next = *root;

    *root = n;
}

int ib_hashmap_drop(ib_hashmap* p, const char* key) {
    ib_hashmap_entry** root = p->buf + (SuperFastHash(key, strlen(key)) % p->len), *cur = *root, *prev = NULL;

    while (cur) {
        if (!strcmp(key, cur->key)) {
            if (prev) {
                prev->next = cur->next;
            } else {
                *root = cur->next;
            }

            ib_free(cur->key);
            ib_free(cur);
            return 0;
        }

        cur = cur->next;
    }

    return 1;
}

void ib_hashmap_foreach(ib_hashmap* p, void (*callback)(const char* key, void* d)) {
    for (int i = 0; i < p->len; ++i) {
        ib_hashmap_entry* cur = p->buf[i];

        while (cur) {
            callback(cur->key, cur->value);
            cur = cur->next;
        }
    }
}
