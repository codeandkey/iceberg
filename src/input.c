#include "input.h"
#include "log.h"
#include "event.h"

#include <SDL2/SDL.h>

static int _ib_input_initialized = 0;
static int _ib_input_keys[1024] = {0};
static int _ib_input_mx, _ib_input_my;

int ib_input_init(void) {
    if (_ib_input_initialized) return ib_warn("already initialized");
    _ib_input_initialized = 1;
    return ib_ok("initialized input");
}

void ib_input_free(void) {
    if (!_ib_input_initialized) return;
    _ib_input_initialized = 0;
}

void ib_input_poll(void) {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        ib_input_event pre = {0};

        switch (e.type) {
        case SDL_QUIT:
            ib_event_add(IB_EVT_QUIT, NULL, 0);
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            pre.keycode = e.key.keysym.sym;
            pre.scancode = e.key.keysym.scancode;

            if (e.type == SDL_KEYDOWN) {
                pre.type = IB_INPUT_EVT_KEYDOWN;
                _ib_input_keys[pre.scancode] = 1;
            }

            if (e.type == SDL_KEYUP) {
                pre.type = IB_INPUT_EVT_KEYUP;
                _ib_input_keys[pre.scancode] = 0;
            }

            ib_event_add(IB_EVT_INPUT, &pre, sizeof pre);
            break;
        case SDL_MOUSEMOTION:
            pre.type = IB_INPUT_EVT_MOUSEPOS;
            _ib_input_mx = pre.x = (int) e.motion.x;
            _ib_input_my = pre.y = (int) e.motion.y;
            ib_event_add(IB_EVT_INPUT, &pre, sizeof pre);
            break;
        case SDL_MOUSEBUTTONDOWN:
            pre.type = IB_INPUT_EVT_MBDOWN;
            pre.button = e.button.button;
            ib_event_add(IB_EVT_INPUT, &pre, sizeof pre);
            break;
        case SDL_MOUSEBUTTONUP:
            pre.type = IB_INPUT_EVT_MBUP;
            pre.button = e.button.button;
            ib_event_add(IB_EVT_INPUT, &pre, sizeof pre);
            break;
        }
    }
}

int ib_input_get_key(int scancode) {
    return _ib_input_keys[scancode];
}

void ib_input_get_mouse(int* mx, int* my) {
    if (mx) *mx = _ib_input_mx;
    if (my) *my = _ib_input_my;
}
