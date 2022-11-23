#pragma once

#include "ecs/system.h"

namespace fuse::ecs {
    class directx_system : public system {
    public:
        FUSE_INLINE void start() override {

        }

        FUSE_INLINE void update(float) override {
            _directx->render_begin();
            _directx->render_end();
        }

    private:

    };
};