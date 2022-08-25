#pragma once

#include "ecs/system.h"

namespace fuse::ecs {

    class rigidbody_system : public system {
    public:
        FUSE_INLINE void update(float dt) {
            for (auto &e: view<rigidbody_component>()) {
                auto &bd = e.get_component<rigidbody_component>();
                if (bd.disabled) continue;

                auto &tr = e.get_component<transform_component>();
                vec2f g = vec2f(0.0f, bd.body.gravity_scale * Gravity);
                bd.body.velocity += (bd.body.force + g) * dt;
                tr.translate += bd.body.velocity * dt;
            }
        }
    private:
        constexpr static const float Gravity = 9.8f;
    };
}