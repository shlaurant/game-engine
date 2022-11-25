#pragma once
#include "asset.h"

namespace fuse{
    struct texture {
        SDL_Texture *data = nullptr;
        std::string filename;
        int height = 0;
        int width = 0;
    };

    struct texture_asset : asset {
        FUSE_INLINE texture_asset() = default;

        FUSE_INLINE ~texture_asset() {
            SDL_DestroyTexture(instance.data);
        }

        texture instance;
    };

    struct texture_dx {
        std::wstring filename;
    };
}
