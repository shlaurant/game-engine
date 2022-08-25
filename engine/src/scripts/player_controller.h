#pragma once

#include "script_instance.h"

namespace fuse {
    class player_controller : public script_instance {

        FUSE_INLINE void on_collision(ecs::entity e) override {
            play_audio("boom");
            Mix_HaltChannel(1);
            auto &sp = get_component<ecs::sprite_component>();
            sp.sprite = get_asset<texture_asset>("dead")->id;
            get_component<ecs::collider_component>().disabled = true;
            get_component<ecs::rigidbody_component>().disabled = true;
        }

        FUSE_INLINE void on_update(float dt) override {
            auto &body = get_component<ecs::rigidbody_component>();
            if (inputs::is_key(SDL_SCANCODE_SPACE)) {
                body.body.set_force_y(force);
            } else {
                body.body.set_force_y(0);
            }
        }

    private:
        float force = -1000.0f;
    };
}