#include <stdio.h>
#include "game.h"
#include "world.h"
#include "audio.h"
#include "graphics/fb.h"

int main(int argc, char** argv) {
    if (ib_game_init()) return 1;

    if (argc > 1) {
        ib_world_load(argv[1]);
    }

    ib_fb* test_fb = ib_fb_alloc(256, 256);
    ib_fb_bind(NULL);

    ib_game_run();

    ib_fb_free(test_fb);

    ib_game_free();
    return 0;
}
