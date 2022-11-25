#pragma once

using namespace DirectX::SimpleMath;

namespace fuse {
    FUSE_INLINE std::vector<vertex> cube(){
        static float d = .5f;
        std::vector<vertex> ret(8);
        ret[0] = {Vector3(-d, d, -d), Vector2(.0f, 1.f)};
        ret[1] = {Vector3(d, d, -d), Vector2(.0f, 1.f)};
        ret[2] = {Vector3(d, d, d), Vector2(.0f, 1.f)};
        ret[3] = {Vector3(-d, d, d), Vector2(.0f, 1.f)};
        ret[4] = {Vector3(-d, -d, -d), Vector2(.0f, 1.f)};
        ret[5] = {Vector3(d, -d, -d), Vector2(.0f, 1.f)};
        ret[6] = {Vector3(d, -d, d), Vector2(.0f, 1.f)};
        ret[7] = {Vector3(-d, -d, d), Vector2(.0f, 1.f)};

        return ret;
    }
}