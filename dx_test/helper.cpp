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
        print_vector4(mult({-1.f, 1.f, -1.f, 1.f}, camera.view() * camera.projection()));
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

DirectX::SimpleMath::Vector4 sub_vec(const Matrix &m, int col){
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
