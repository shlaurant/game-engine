#include <core/entry.h>
#include <scripts/script_factory.h>
#include "scripts/pipe_spawner.h"
#include "scripts/player_controller.h"
#include "scripts/scrolling_ground.h"

fuse::app_config fuse::create_application() {
    fuse::app_config config;
    config.scene="assets/scene.yaml";
    config.title = "Flappy Plane";
    config.version = "0.0.1";
    config.width = 380;
    config.height = 720;

    REGISTER_SCRIPT(pipe_spawner)
    REGISTER_SCRIPT(player_controller)
    REGISTER_SCRIPT(scrolling_ground)

    return config;
}