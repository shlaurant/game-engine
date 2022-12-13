#pragma once

#include "SimpleMath.h"

namespace fuse::directx {
    struct vertex {
        DirectX::SimpleMath::Vector3 position;
//        DirectX::SimpleMath::Vector4 color;
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

    struct light {
        enum type {
            DIRECTIONAL, POINT, SPOT
        };

        int light_type;
        DirectX::SimpleMath::Vector3 color;
        float fo_start;
        DirectX::SimpleMath::Vector3 direction;
        float fo_end;
        DirectX::SimpleMath::Vector3 position;
        float spot_pow;
        DirectX::SimpleMath::Vector3 pad0;
    };

    struct material {
        DirectX::SimpleMath::Vector4 diffuse_albedo;
        DirectX::SimpleMath::Vector3 fresnel_r0;
        float roughness;
    };
}