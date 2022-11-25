#pragma once

#include "asset.h"
#include "directx/directx_12.h"

namespace fuse {
    struct mesh_asset : public asset {
        std::vector<vertex> vertices;
    };
}