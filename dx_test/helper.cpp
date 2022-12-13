#include <SimpleMath.h>
#include <string>
#include "helper.h"

using namespace DirectX::SimpleMath;

Matrix transform::translation_matrix() const {
    return Matrix::CreateTranslation(position);
}

Matrix transform::rotation_matrix() const {
    Matrix ret = Matrix::Identity;
    ret *= Matrix::CreateRotationX(rotation.x);
    ret *= Matrix::CreateRotationY(rotation.y);
    ret *= Matrix::CreateRotationZ(rotation.z);
    return ret;
}

Matrix transform::scale_matrix() const {
    return Matrix::CreateScale(scale);
}

DirectX::SimpleMath::Matrix camera::view() const {
    return (transform.rotation_matrix() *
            transform.translation_matrix()).Invert();
}

DirectX::SimpleMath::Matrix camera::projection() const {
    return DirectX::XMMatrixPerspectiveFovLH(fov, ratio(), near_plane,
                                             far_plane);
}

void handle_input(Input &input, camera &camera) {
    if (input.GetButton(KEY_TYPE::RIGHT)) {
        camera.transform.position.x += .1f;
    }

    if (input.GetButton(KEY_TYPE::UP)) {
        camera.transform.position.y += .1f;
    }

    if (input.GetButton(KEY_TYPE::DOWN)) {
        camera.transform.position.y -= .1f;
    }

    if (input.GetButton(KEY_TYPE::LEFT)) {
        camera.transform.position.x -= .1f;
    }

    if (input.GetButton(KEY_TYPE::W)) {
        camera.transform.position.z += .1f;
    }

    if (input.GetButton(KEY_TYPE::S)) {
        camera.transform.position.z -= .1f;
    }

    if (input.GetButton(KEY_TYPE::A)) {
        camera.transform.rotation.y -= .1f;
    }

    if (input.GetButton(KEY_TYPE::D)) {
        camera.transform.rotation.y += .1f;
    }

    if (input.GetButtonDown(KEY_TYPE::Q)) {
        print_transform(camera.transform);
        OutputDebugStringA("view:\n");
        print_matrix(camera.view());
        OutputDebugStringA("projection:\n");
        print_matrix(camera.projection());
        OutputDebugStringA("vp:\n");
        print_matrix(camera.view() * camera.projection());
        OutputDebugStringA("test mult:\n");
        print_vector4(mult({-1.f, 1.f, -1.f, 1.f},
                           camera.view() * camera.projection()));
    }
}

void print_matrix(const Matrix &matrix) {
    auto m = matrix.m;
    for (auto i = 0; i < 4; ++i) {
        std::string str;
        for (auto e: m[i]) {
            str += std::to_string(e);
            str += " ";
        }
        str += "\n";
        OutputDebugStringA(str.c_str());
    }
}

void print_vector3(const Vector3 &v) {
    std::string str;

    str += std::to_string(v.x) + " ";
    str += std::to_string(v.y) + " ";
    str += std::to_string(v.z) + "\n";

    OutputDebugStringA(str.c_str());
}

void print_vector4(const Vector4 &v) {
    std::string str;

    str += std::to_string(v.x) + " ";
    str += std::to_string(v.y) + " ";
    str += std::to_string(v.z) + " ";
    str += std::to_string(v.w) + "\n";

    OutputDebugStringA(str.c_str());
}

void print_transform(const transform &t) {
    OutputDebugStringA("transform:\n");
    OutputDebugStringA("scale   : ");
    print_vector3(t.scale);
    OutputDebugStringA("rotation: ");
    print_vector3(t.rotation);
    OutputDebugStringA("position: ");
    print_vector3(t.position);
}

DirectX::SimpleMath::Vector4 sub_vec(const Matrix &m, int col) {
    Vector4 ret;
    ret.x = m.m[0][col];
    ret.y = m.m[1][col];
    ret.z = m.m[2][col];
    ret.w = m.m[3][col];
    return ret;
}

DirectX::SimpleMath::Vector4 mult(const Vector4 &v, const Matrix &m) {
    Vector4 ret;

    ret.x = v.Dot(sub_vec(m, 0));
    ret.y = v.Dot(sub_vec(m, 1));
    ret.z = v.Dot(sub_vec(m, 2));
    ret.w = v.Dot(sub_vec(m, 3));

    return ret;
}

fuse::directx::geometry create_cube() {
    static const float d = 0.5f;
    fuse::directx::geometry ret;
    ret.vertices = {{{-d, d,  -d}, white()},
                    {{d,  d,  -d}, red()},
                    {{-d, -d, -d}, green()},
                    {{d,  -d, -d}, blue()},
                    {{-d, d,  d},  white()},
                    {{d,  d,  d},  red()},
                    {{-d, -d, d},  green()},
                    {{d,  -d, d},  blue()}};
    ret.indices = {0, 1, 2, 1, 3, 2,
                   1, 7, 3, 1, 5, 7,
                   0, 4, 1, 4, 5, 1,
                   0, 2, 6, 0, 6, 4,
                   2, 3, 6, 3, 7, 6,
                   4, 6, 5, 5, 6, 7};

    return ret;
}

fuse::directx::geometry create_tetra() {
    static const float d = 0.5f;
    fuse::directx::geometry ret;
    ret.vertices = {{{0.f, d * 2, 0.f}, red()},
                    {{0.f, 0.f,   d},   green()},
                    {{d,   0.f,   -d},  blue()},
                    {{-d,  0.f,   -d},  white()}};
    ret.indices = {0, 2, 3,
                   0, 1, 2,
                   0, 3, 1,
                   2, 1, 3};
    return ret;
}

fuse::directx::geometry create_cube_uv() {
    fuse::directx::geometry ret;

    float w2 = 0.5f;
    float h2 = 0.5f;
    float d2 = 0.5f;

    std::vector<fuse::directx::vertex> vec(24);

    vec[0] = {Vector3(-w2, -h2, -d2), Vector2(0.0f, 1.0f), Vector3(0.f, 0.f, -1.f)};
    vec[1] = {Vector3(-w2, +h2, -d2), Vector2(0.0f, 0.0f), Vector3(0.f, 0.f, -1.f)};
    vec[2] = {Vector3(+w2, +h2, -d2), Vector2(1.0f, 0.0f), Vector3(0.f, 0.f, -1.f)};
    vec[3] = {Vector3(+w2, -h2, -d2), Vector2(1.0f, 1.0f), Vector3(0.f, 0.f, -1.f)};
    vec[4] = {Vector3(-w2, -h2, +d2), Vector2(1.0f, 1.0f), Vector3(0.f, 0.f, 1.f)};
    vec[5] = {Vector3(+w2, -h2, +d2), Vector2(0.0f, 1.0f), Vector3(0.f, 0.f, 1.f)};
    vec[6] = {Vector3(+w2, +h2, +d2), Vector2(0.0f, 0.0f), Vector3(0.f, 0.f, 1.f)};
    vec[7] = {Vector3(-w2, +h2, +d2), Vector2(1.0f, 0.0f), Vector3(0.f, 0.f, 1.f)};
    vec[8] = {Vector3(-w2, +h2, -d2), Vector2(0.0f, 1.0f), Vector3(0.f, 1.f, 0.f)};
    vec[9] = {Vector3(-w2, +h2, +d2), Vector2(0.0f, 0.0f), Vector3(0.f, 1.f, 0.f)};
    vec[10] = {Vector3(+w2, +h2, +d2), Vector2(1.0f, 0.0f), Vector3(0.f, 1.f, 0.f)};
    vec[11] = {Vector3(+w2, +h2, -d2), Vector2(1.0f, 1.0f), Vector3(0.f, 1.f, 0.f)};
    vec[12] = {Vector3(-w2, -h2, -d2), Vector2(1.0f, 1.0f), Vector3(0.f, -1.f, 0.f)};
    vec[13] = {Vector3(+w2, -h2, -d2), Vector2(0.0f, 1.0f), Vector3(0.f, -1.f, 0.f)};
    vec[14] = {Vector3(+w2, -h2, +d2), Vector2(0.0f, 0.0f), Vector3(0.f, -1.f, 0.f)};
    vec[15] = {Vector3(-w2, -h2, +d2), Vector2(1.0f, 0.0f), Vector3(0.f, -1.f, 0.f)};
    vec[16] = {Vector3(-w2, -h2, +d2), Vector2(0.0f, 1.0f), Vector3(-1.f, 0.f, 0.f)};
    vec[17] = {Vector3(-w2, +h2, +d2), Vector2(0.0f, 0.0f), Vector3(-1.f, 0.f, 0.f)};
    vec[18] = {Vector3(-w2, +h2, -d2), Vector2(1.0f, 0.0f), Vector3(-1.f, 0.f, 0.f)};
    vec[19] = {Vector3(-w2, -h2, -d2), Vector2(1.0f, 1.0f), Vector3(-1.f, 0.f, 0.f)};
    vec[20] = {Vector3(+w2, -h2, -d2), Vector2(0.0f, 1.0f), Vector3(1.f, 0.f, 0.f)};
    vec[21] = {Vector3(+w2, +h2, -d2), Vector2(0.0f, 0.0f), Vector3(1.f, 0.f, 0.f)};
    vec[22] = {Vector3(+w2, +h2, +d2), Vector2(1.0f, 0.0f), Vector3(1.f, 0.f, 0.f)};
    vec[23] = {Vector3(+w2, -h2, +d2), Vector2(1.0f, 1.0f), Vector3(1.f, 0.f, 0.f)};

    std::vector<uint16_t> idx(36);

    idx[0] = 0;
    idx[1] = 1;
    idx[2] = 2;
    idx[3] = 0;
    idx[4] = 2;
    idx[5] = 3;
    idx[6] = 4;
    idx[7] = 5;
    idx[8] = 6;
    idx[9] = 4;
    idx[10] = 6;
    idx[11] = 7;
    idx[12] = 8;
    idx[13] = 9;
    idx[14] = 10;
    idx[15] = 8;
    idx[16] = 10;
    idx[17] = 11;
    idx[18] = 12;
    idx[19] = 13;
    idx[20] = 14;
    idx[21] = 12;
    idx[22] = 14;
    idx[23] = 15;
    idx[24] = 16;
    idx[25] = 17;
    idx[26] = 18;
    idx[27] = 16;
    idx[28] = 18;
    idx[29] = 19;
    idx[30] = 20;
    idx[31] = 21;
    idx[32] = 22;
    idx[33] = 20;
    idx[34] = 22;
    idx[35] = 23;

    ret.vertices = vec;
    ret.indices = idx;

    return ret;
}

DirectX::SimpleMath::Vector4 white() {
    return {1.f, 1.f, 1.f, 1.f};
}
DirectX::SimpleMath::Vector4 red() {
    return {1.f, 0.f, 0.f, 1.f};
}
DirectX::SimpleMath::Vector4 green() {
    return {0.f, 1.f, 0.f, 1.f};
}
DirectX::SimpleMath::Vector4 blue() {
    return {0.f, 0.f, 1.f, 1.f};
}
