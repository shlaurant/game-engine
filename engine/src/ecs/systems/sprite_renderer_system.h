#pragma once
#include "ecs/system.h"

namespace fuse::ecs {
    class sprite_renderer_system : public system {
    public:
        FUSE_INLINE void update(float) {
            for (auto &e: view<sprite_component>()) {
                auto &tr = e.get_component<transform_component>();
                auto &sp = e.get_component<sprite_component>();

                auto &sprite = _assets->get<texture_asset>(sp.sprite)->instance;

                SDL_FRect dst{
                        tr.translate.x, tr.translate.y,
                        sprite.width * tr.scale.x, sprite.height * tr.scale.y
                };

                SDL_RenderCopyExF(_renderer, sprite.data, nullptr, &dst,
                                  tr.rotation, nullptr, sp.flip);
            }
        }
    };
}