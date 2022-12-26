#pragma once

#include "SimpleMath.h"

namespace fuse::directx {
    struct render_info {
        int object_index;
        int index_count;
        int index_offset;
        int vertex_offset;
        bool is_transparent;
        bool do_reflect;
        bool is_mirror;
        DirectX::SimpleMath::Vector4 mirror_plane;
    };

    struct vertex {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector2 uv;
        DirectX::SimpleMath::Vector3 normal;
    };

    struct geometry {
        std::vector<vertex> vertices;
        std::vector<uint16_t> indices;

        DirectX::SimpleMath::Matrix world_matrix = DirectX::SimpleMath::Matrix::Identity;

        size_t vertex_offset = 0;
        size_t index_offset = 0;
        size_t w_offset = 0;
    };

    struct global {
        DirectX::SimpleMath::Matrix reflection_matrix[10];
        int reflection_count;
        DirectX::SimpleMath::Vector3 pad0;
    };

    struct material {
        DirectX::SimpleMath::Vector4 diffuse_albedo;
        DirectX::SimpleMath::Vector3 fresnel_r0;
        float roughness;
    };

    struct object_constant {
        DirectX::SimpleMath::Matrix world_matrix;
        material material;
    };

    struct camera {
        DirectX::SimpleMath::Matrix vp;
        DirectX::SimpleMath::Vector3 position;
        float pad0;
    };

    enum light_type {
        DIRECTIONAL = 0, POINT, SPOT, AMBIENT, NONE
    };

    struct light {
        int type = (int) light_type::NONE;
        DirectX::SimpleMath::Vector3 color;
        float fo_start;
        DirectX::SimpleMath::Vector3 direction;
        float fo_end;
        DirectX::SimpleMath::Vector3 position;
        float spot_pow;
        DirectX::SimpleMath::Vector3 pad0;
    };

    struct light_info {
        light lights[50];//should be same with LIGHT_COUNT in shader code
        int active_count;
        DirectX::SimpleMath::Vector3 pad0;
    };
}