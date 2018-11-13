#include "obj.h"
#include "../world.h"
#include "../log.h"

#include "obj_snow.h"
#include "obj_player.h"
#include "obj_light.h"
#include "obj_grenade.h"
#include "obj_explosion.h"
#include "obj_player_trail.h"
#include "obj_noclip.h"
#include "obj_enemy.h"
#include "obj_fog.h"
#include "obj_bg.h"

/* include example object type */
#include "obj_example.h"

void ib_obj_bind_all() {
    ib_ok("binding object types");

    /* ----
     * here all object types are bound. add your object here with its init/destroy functions
     * to add it to the game.
     * ---- */

    ib_world_bind_object("bg", obj_bg_init, obj_bg_destroy, obj_bg_evt);
    ib_world_bind_object("snow", obj_snow_init, obj_snow_destroy, obj_snow_evt);
    ib_world_bind_object("player", obj_player_init, obj_player_destroy, obj_player_evt);
    ib_world_bind_object("light", obj_light_init, obj_light_destroy, obj_light_evt);
    ib_world_bind_object("grenade", obj_grenade_init, obj_grenade_destroy, obj_grenade_evt);
    ib_world_bind_object("explosion", obj_explosion_init, obj_explosion_destroy, obj_explosion_evt);
    ib_world_bind_object("player_trail", obj_player_trail_init, obj_player_trail_destroy, obj_player_trail_evt);
    ib_world_bind_object("noclip", obj_noclip_init, obj_noclip_destroy, obj_noclip_evt);
    ib_world_bind_object("enemy", obj_enemy_init, obj_enemy_destroy, obj_enemy_evt);
    ib_world_bind_object("fog", obj_fog_init, obj_fog_destroy, obj_fog_evt);

    /* bind an object type with a typename, init function, and destroy function as declared in its header */
    ib_world_bind_object("example", obj_example_init, obj_example_destroy, obj_example_evt);
}
