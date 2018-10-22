#include "timer.h"

#include <SDL2/SDL.h>

uint32_t ib_timer_point() {
    return SDL_GetTicks();
}

int ib_timer_ms(ib_timepoint p) {
    return SDL_GetTicks() - p;
}
