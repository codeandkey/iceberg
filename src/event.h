#ifndef IB_EVENT
#define IB_EVENT

#define IB_EVENT_MAX 256

/* event types */
#define IB_EVT_UPDATE 0
#define IB_EVT_DRAW 1
#define IB_EVT_QUIT 2
#define IB_EVT_INPUT 3
#define IB_EVT_DRAW_WEATHER 4
#define IB_EVT_DRAW_WORLD 5
#define IB_EVT_DRAW_BACKGROUND 6
#define IB_EVT_DRAW_BACKGROUND_POST 7
#define IB_EVT_DRAW_WORLD_LIGHTS 8

/* game event types */
#define IB_EVT_GAME_EXPLOSION 128

/* end event types */

typedef struct _ib_event {
    int type;
    void* evt;
} ib_event;

typedef int (*ib_event_cb)(ib_event* const e, void* d);

int ib_event_init();
void ib_event_free();

int ib_event_subscribe(int type, ib_event_cb cb, void* d);
int ib_event_unsubscribe(int id);

int ib_event_add(int type, void* evt, int evtsize); /* evt is copied in */
void ib_event_send(ib_event* e);
void ib_event_work(); /* work to clear the queue of events. */

#endif
