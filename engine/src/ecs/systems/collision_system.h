#pragma once

#include "ecs/system.h"

namespace fuse::ecs {
    class collision_system : public system {
    public:
        FUSE_INLINE void start() {
            for (auto &e: view<collision_system>()) {
                auto &col = e.get_component<collider_component>();
                auto &tr = e.get_component<transform_component>();
                col.collider.x = tr.translate.x;
                col.collider.y = tr.translate.y;
            }
        }

        FUSE_INLINE void update(float) {
            auto entities = view<collider_component>();

            for (auto &e: entities) {
                auto &tr = e.get_component<transform_component>();
                auto &col = e.get_component<collider_component>();

                col.collider.x = tr.translate.x;
                col.collider.y = tr.translate.y;

                if (col.disabled) { continue; }

                for (auto &o: entities) {
                    if (e == o) { continue; }
                    auto &oc = o.get_component<collider_component>();

                    if (check_collision(col.collider, oc.collider)) {
                        e.get_component<rigidbody_component>().body.gravity_scale = 1;
                    }
                }
            }
        }
    private:
        bool check_collision(const SDL_FRect &a, const SDL_FRect &b) {
            return (a.x < b.x + b.w) && (a.x + a.w > b.x) &&
                   (a.y < b.y + b.h) && (a.y + a.h > b.y);
        }
    };
}