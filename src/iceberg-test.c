#include <stdio.h>
#include "game.h"

int main(int argc, char** argv) {
    if (ib_game_init()) return 1;
    ib_game_run();
    ib_game_free();
    return 0;
}
