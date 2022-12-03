#pragma once

#include "Input.h"

struct transform {
    DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 rotation = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3::One;

    DirectX::SimpleMath::Matrix translation_matrix() const;
    DirectX::SimpleMath::Matrix rotation_matrix() const;
    DirectX::SimpleMath::Matrix scale_matrix() const;
};

struct camera {
    inline static float ratio() { return 1920.f / 1080.f; }

    transform transform;

    float near_plane = 2.f;
    float far_plane = 100.f;
    float fov = DirectX::XM_PI / 2.f;

    DirectX::SimpleMath::Matrix view() const;
    DirectX::SimpleMath::Matrix projection() const;
};

void handle_input(Input &, camera &);

void print_matrix(const DirectX::SimpleMath::Matrix &);

void print_vector3(const DirectX::SimpleMath::Vector3 &);
void print_vector4(const DirectX::SimpleMath::Vector4 &);

void print_transform(const transform &);

DirectX::SimpleMath::Vector4 mult(const DirectX::SimpleMath::Vector4 &, const DirectX::SimpleMath::Matrix &);