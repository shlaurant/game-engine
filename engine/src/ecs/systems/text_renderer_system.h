#pragma once

#include "ecs/system.h"

namespace fuse::ecs {
    class text_renderer_system : public system {
    public:
        FUSE_INLINE void update(float) {
            for (auto &e: view<text_component>()) {
                auto &tr = e.get_component<transform_component>();
                auto &tx = e.get_component<text_component>();

                auto &font = _assets->get<font_asset>(tx.font)->instance;

                SDL_Surface *surface = nullptr;
                surface = TTF_RenderText_Blended(font.data, tx.text.c_str(),
                                                 tx.color);
                if (!surface) {
                    FUSE_ERROR("%s", TTF_GetError());
                    continue;
                }

                SDL_Texture *texture = SDL_CreateTextureFromSurface(_renderer,
                                                                    surface);
                SDL_FreeSurface(surface);
                if (!texture) {
                    FUSE_ERROR("%s", SDL_GetError());
                    continue;
                }

                int width, height;
                SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

                SDL_FRect dst{
                        tr.translate.x, tr.translate.y, width * tr.scale.x,
                        height * tr.scale.y
                };

                SDL_RenderCopyExF(_renderer, texture, nullptr, &dst,
                                  tr.rotation, nullptr, tx.flip);
                SDL_DestroyTexture(texture);
            }
        }
    };
}