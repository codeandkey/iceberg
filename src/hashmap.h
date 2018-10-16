#ifndef IB_HASHMAP
#define IB_HASHMAP

/* the hashmap does NOT copy string contents (for now), so only use it with string literals */

typedef struct _ib_hashmap_entry {
    char* key;
    void* value;
    struct _ib_hashmap_entry* next;
} ib_hashmap_entry;

typedef struct _ib_hashmap {
    ib_hashmap_entry** buf;
    int len;
} ib_hashmap;

ib_hashmap* ib_hashmap_alloc(int table_size);
void ib_hashmap_free(ib_hashmap* p);

void* ib_hashmap_get(ib_hashmap* p, const char* key);
void ib_hashmap_set(ib_hashmap* p, const char* key, void* value);
int ib_hashmap_drop(ib_hashmap* p, const char* key);
void ib_hashmap_foreach(ib_hashmap* p, void (*callback)(const char* key, void* v));

#endif
