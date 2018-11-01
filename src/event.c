#include "event.h"
#include "log.h"
#include "mem.h"

#include <stdlib.h>
#include <string.h>

typedef struct _ib_event_sub {
    ib_event_cb cb;
    void* data;
    int id;
    struct _ib_event_sub* next;
} ib_event_sub;

typedef struct _ib_event_entry {
    ib_event e;
    struct _ib_event_entry* next;
} ib_event_entry;

static ib_event_sub* _ib_event_subs[IB_EVENT_MAX];
static ib_event_entry* _ib_event_list, *_ib_event_tail;
static int _ib_event_id = 0;

int ib_event_init() {
    /* maybe clear the subs but we don't really have to */
    return ib_ok("initialized event system");
}

void ib_event_free() {
    ib_event_sub* tmp;

    for (int i = 0; i < IB_EVENT_MAX; ++i) {
        while (_ib_event_subs[i]) {
            tmp = _ib_event_subs[i]->next;
            ib_free(_ib_event_subs[i]);
            _ib_event_subs[i] = tmp;
        }
    }
}

int ib_event_subscribe(int type, ib_event_cb cb, void* d) {
    ib_event_sub* s = ib_malloc(sizeof *s);

    s->cb = cb;
    s->data = d;
    s->id = _ib_event_id++;
    s->next = _ib_event_subs[type];

    _ib_event_subs[type] = s;
    return s->id;
}

int ib_event_unsubscribe(int id) {
    for (int i = 0; i < IB_EVENT_MAX; ++i) {
        ib_event_sub* cur = _ib_event_subs[i], *prev = NULL;

        while (cur) {
            if (cur->id == id) {
                if (prev) {
                    prev->next = cur->next;
                } else {
                    _ib_event_subs[i] = cur->next;
                }

                ib_free(cur);
                return 0;
            }

            prev = cur;
            cur = cur->next;
        }
    }

    return ib_warn("unsubscribe failed for id %d", id);
}

void ib_event_send(ib_event* e) {
    ib_event_sub* cur = _ib_event_subs[e->type], *tmp;

    while (cur) {
        tmp = cur->next;
        cur->cb(e, cur->data); /* cur could be destroyed mid event so we store a tmp */
        cur = tmp;
    }
}

void ib_event_work() {
    ib_event_entry* tmp;

    while (_ib_event_list) {
        ib_event_send(&_ib_event_list->e);

        tmp = _ib_event_list->next;
        ib_free(_ib_event_list->e.evt);
        ib_free(_ib_event_list);
        _ib_event_list = tmp;
    }

    _ib_event_tail = NULL;
}

int ib_event_add(int type, void* d, int len) {
    ib_event_entry* n = ib_malloc(sizeof *n);

    n->e.evt = d ? ib_malloc(len) : NULL;
    n->e.type = type;
    n->next = NULL;

    if (n->e.evt) {
        memcpy(n->e.evt, d, len);
    }

    if (_ib_event_tail) {
        _ib_event_tail->next = n;
    } else {
        _ib_event_list = n;
    }

    _ib_event_tail = n;
    return 0;
}
