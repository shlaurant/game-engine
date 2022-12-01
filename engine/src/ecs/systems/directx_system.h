#pragma once

#include "ecs/system.h"

namespace fuse::ecs {
    class directx_system : public system {
    public:
        FUSE_INLINE void start() override {
//            static const float d = 0.1f;
//            geometry t;
//            DirectX::SimpleMath::Vector4 white = {1.f, 1.f, 1.f, 1.f};
//            t.vertices = {{{-d, d,  -d}, white},
//                          {{d,  d,  -d}, white},
//                          {{-d, -d, -d}, white},
//                          {{d,  -d, -d}, white},
//                          {{-d, d,  d},  white},
//                          {{d,  d,  d},  white},
//                          {{-d, -d, d},  white},
//                          {{d,  -d, d},  white}};
//            t.indices = {0, 1, 2, 1, 3, 2,
//                         4, 1, 0, 4, 5, 1,
//                         4, 6, 5, 5, 6, 7,
//                         6, 2, 3, 6, 3, 7,
//                         1, 7, 3, 5, 7, 1,
//                         2, 6, 0, 0, 6, 4};
//            _test.push_back(t);
//            _directx->init_geometries(_test);
        }

        FUSE_INLINE void update(float) override {
//            _directx->render_begin();
//            for(const auto &e:_test){
//                _directx->render(e);
//            }
//            _directx->render_end();
        }
    };
};