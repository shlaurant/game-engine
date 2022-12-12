#pragma once

#include "SimpleMath.h"

namespace fuse::directx {
    struct vertex {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector4 color;
        DirectX::SimpleMath::Vector2 uv;
    };

    struct geometry {
        std::vector<vertex> vertices;
        std::vector<uint16_t> indices;

        DirectX::SimpleMath::Matrix world_matrix = DirectX::SimpleMath::Matrix::Identity;

        size_t vertex_offset = 0;
        size_t index_offset = 0;
        size_t w_offset = 0;
    };

    struct texture {
        std::wstring filename;

    };
}