#pragma once

#include "SimpleMath.h"

namespace fuse::directx {
    struct transform {
        DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Vector3 rotation = DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3::One;

        inline DirectX::SimpleMath::Matrix world_matrix() {
            return DirectX::SimpleMath::Matrix::CreateScale(scale) *
                   DirectX::SimpleMath::Matrix::CreateRotationX(rotation.x) *
                   DirectX::SimpleMath::Matrix::CreateRotationY(rotation.y) *
                   DirectX::SimpleMath::Matrix::CreateRotationY(rotation.z) *
                   DirectX::SimpleMath::Matrix::CreateTranslation(position);
        }
    };

    struct render_info {
        bool is_billboard;
        int object_index;
        int index_count;
        int index_offset;
        int vertex_offset;
        bool is_transparent;
        bool do_reflect;
        bool is_mirror;
        DirectX::SimpleMath::Vector4 mirror_plane;
        bool do_shadow;
        bool is_terrain = false;
    };

    struct vertex {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector2 uv;
        DirectX::SimpleMath::Vector3 normal;
    };

    struct vertex_billboard {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector2 size;
    };

    template<typename T>
    struct geometry {
        std::string name;
        std::vector<T> vertices;
        std::vector<uint16_t> indices;

        DirectX::SimpleMath::Matrix world_matrix = DirectX::SimpleMath::Matrix::Identity;

        size_t vertex_offset = 0;
        size_t index_offset = 0;
    };

    struct material {
        DirectX::SimpleMath::Vector4 diffuse_albedo;
        DirectX::SimpleMath::Vector3 fresnel_r0;
        float roughness;
    };

    struct object_constant {
        DirectX::SimpleMath::Vector3 position;
        int mat_id;
        DirectX::SimpleMath::Matrix world_matrix;
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

    struct global {
        DirectX::SimpleMath::Matrix reflection_matrix[10];
        int reflection_count;
        DirectX::SimpleMath::Vector3 pad0;
        DirectX::SimpleMath::Matrix shadow_matrix;
    };
}