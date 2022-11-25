#pragma once

#include "SimpleMath.h"

namespace fuse {
    struct vertex {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector4 color;
    };

    struct geometry {
        std::vector<vertex> vertices;
        std::vector<uint16_t> indices;

        size_t vertex_offset = 0;
        size_t index_offset = 0;
    };
}