#pragma once

#include "ecs/system.h"

namespace fuse::ecs {
    class frame_animation_system : public system {
    public:
        FUSE_INLINE void update(float) {
            for (auto &e: view<animation_component>()) {
                auto &tr = e.get_component<transform_component>();
                auto &an = e.get_component<animation_component>();

                auto &anim = _assets->get<animation_asset>(
                        an.animation)->instance;

                int index = (get_ticks_ms() / anim.speed) % anim.frames.size();
                auto &frame = _assets->get<texture_asset>(
                        anim.frames[index])->instance;

                SDL_FRect dst{
                        tr.translate.x, tr.translate.y,
                        frame.width * tr.scale.x, frame.height * tr.scale.y
                };

                SDL_RenderCopyExF(_renderer, frame.data, nullptr, &dst,
                                  tr.rotation, nullptr, an.flip);
            }
        }
    };
}