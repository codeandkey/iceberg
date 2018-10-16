#ifndef IB_INPUT
#define IB_INPUT

/* bring in keycodes so dependents can know key types */
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

#define IB_INPUT_EVT_KEYDOWN 0
#define IB_INPUT_EVT_KEYUP 1
#define IB_INPUT_EVT_MBDOWN 2
#define IB_INPUT_EVT_MBUP 3
#define IB_INPUT_EVT_MOUSEPOS 4

typedef struct _ib_input_event {
    int type;
    int keycode, scancode; /* keyboard values */
    int button, x, y; /* mouse values */
} ib_input_event;

int ib_input_init(void);
void ib_input_free(void);
void ib_input_poll(void);

#endif
