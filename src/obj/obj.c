#include "obj.h"
#include "../world.h"
#include "../log.h"

#include "obj_bg.h"
#include "obj_snow.h"
#include "obj_fog.h"
#include "obj_player.h"
#include "obj_light.h"
#include "obj_grenade.h"
#include "obj_explosion.h"

void ib_obj_bind_all() {
    ib_ok("binding object types");

    /* ----
     * here all object types are bound. add your object here with its init/destroy functions
     * to add it to the game.
     * ---- */

    ib_world_bind_object("bg", obj_bg_init, obj_bg_destroy);
    ib_world_bind_object("snow", obj_snow_init, obj_snow_destroy);
    ib_world_bind_object("fog", obj_fog_init, obj_fog_destroy);
    ib_world_bind_object("player", obj_player_init, obj_player_destroy);
    ib_world_bind_object("light", obj_light_init, obj_light_destroy);
    ib_world_bind_object("grenade", obj_grenade_init, obj_grenade_destroy);
    ib_world_bind_object("explosion", obj_explosion_init, obj_explosion_destroy);
}
