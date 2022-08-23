#pragma once
#include "assets/asset.h"

namespace fuse::ecs {
    struct sprite_component {
        FUSE_INLINE sprite_component(const sprite_component &) = default;
        FUSE_INLINE sprite_component() = default;

        SDL_RendererFlip flip = SDL_FLIP_NONE;
        asset_id sprite = INVALID_ID;
    };
}