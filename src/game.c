#include "game.h"
#include "mem.h"
#include "log.h"
#include "config.h"
#include "graphics.h"
#include "event.h"
#include "input.h"
#include "world.h"
#include "audio.h"
#include "obj/obj.h"

static int _ib_game_should_quit;
static int _ib_game_keys[2048];

int _ib_game_key_handler(ib_event* e, void* d) {
    ib_input_event* ie = (ib_input_event*) e->evt;

    if (ie->type == IB_INPUT_EVT_KEYDOWN && ie->keycode == SDLK_ESCAPE) {
        ib_ok("user pressed escape, quitting");
        ib_event_add(IB_EVT_QUIT, NULL, 0);
    }

    if (ie->type == IB_INPUT_EVT_KEYDOWN) {
        _ib_game_keys[ie->scancode] = 1;
    }

    if (ie->type == IB_INPUT_EVT_KEYUP) {
        _ib_game_keys[ie->scancode] = 0;
    }

    return 0;
}

int _ib_game_update_handler(ib_event* e, void* d) {
    ib_event_update* u = e->evt;
    float dt_s = u->dt / 1000.0f;

    int cx, cy;
    ib_graphics_get_camera(&cx, &cy);

    if (_ib_game_keys[SDL_SCANCODE_LEFT]) {
        cx -= (dt_s * 50.0f) + 1;
    }

    if (_ib_game_keys[SDL_SCANCODE_RIGHT]) {
        cx += (dt_s * 50.0f) + 1;
    }

    if (_ib_game_keys[SDL_SCANCODE_UP]) {
        cy -= (dt_s * 50.0f) + 1;
    }

    if (_ib_game_keys[SDL_SCANCODE_DOWN]) {
        cy += (dt_s * 50.0f) + 1;
    }

    ib_graphics_set_camera(cx, cy);
}

int _ib_game_quit_handler(ib_event* e, void* d) {
    _ib_game_should_quit = 1;
    return 0;
}

int ib_game_init(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) return ib_err("SDL init failed");

    if (ib_config_init()) return ib_err("ib_config failed");
    if (ib_graphics_init()) return ib_err("ib_graphics failed");
    if (ib_audio_init()) return ib_err("ib_audio failed");
    if (ib_event_init()) return ib_err("ib_event failed");
    if (ib_input_init()) return ib_err("ib_input failed");
    if (ib_world_init()) return ib_err("ib_world failed");

    /* load object types */
    ib_obj_bind_all();

    if (ib_world_load(IB_WORLD_DEFAULT)) return ib_err("default world failed");

    /* register global key handler */
    ib_event_subscribe(IB_EVT_INPUT, _ib_game_key_handler, NULL);

    /* register quit event */
    ib_event_subscribe(IB_EVT_QUIT, _ib_game_quit_handler, NULL);

    ib_event_subscribe(IB_EVT_UPDATE, _ib_game_update_handler, NULL);

    /* start bgm */
    ib_audio_bgm_add(IB_AUDIO_AUDIOFILE("bgm"));

    /* create test object */
    ib_world_create_object("snow", NULL, NULL);
    ib_world_create_object("fog", NULL, NULL);

    ib_hashmap* tprops = ib_hashmap_alloc(4);
    ib_hashmap_set(tprops, "echo", "obj_test test message!");
    ib_world_create_object("test", NULL, tprops);

    ib_world_create_object("bg", NULL, NULL);

    return ib_ok("initialized game");
}

void ib_game_free(void) {
    ib_world_free();
    ib_input_free();
    ib_event_free();
    ib_audio_free();
    ib_graphics_free();
    ib_config_free();

    SDL_Quit();
}

int ib_game_run(void) {
    ib_ok("starting main loop");

    uint32_t ticks, frame_dt;
    ticks = SDL_GetTicks();

    _ib_game_should_quit = 0;

    while (!_ib_game_should_quit) {
        frame_dt = SDL_GetTicks() - ticks;
        ticks += frame_dt;

        ib_audio_update(frame_dt);
        ib_input_poll();
        ib_graphics_clear();

        ib_event_update ud;
        ud.dt = frame_dt;

        ib_event_add(IB_EVT_DRAW, NULL, 0);
        ib_event_add(IB_EVT_UPDATE, &ud, sizeof ud);
        ib_event_work();

        ib_world_update_animations(frame_dt);
        ib_world_render();
        ib_graphics_swap();
    }

    return ib_ok("terminating main loop");
}
