#include <stdio.h>
#include "game.h"
#include "world.h"
#include "audio.h"

int main(int argc, char** argv) {
    if (ib_game_init()) return 1;

    ib_world_create_object("snow", NULL, NULL);
    ib_world_create_object("fog", NULL, NULL);
    ib_world_create_object("bg", NULL, NULL);

    ib_audio_bgm_add(IB_AUDIO_AUDIOFILE("bgm"));

    ib_game_run();
    ib_game_free();
    return 0;
}
