#include <stdio.h>
#include "game.h"
#include "world.h"
#include "audio.h"

int main(int argc, char** argv) {
    if (ib_game_init()) return 1;

    if (argc > 1) {
        ib_world_load(argv[1]);
    }

    ib_game_run();
    ib_game_free();
    return 0;
}
