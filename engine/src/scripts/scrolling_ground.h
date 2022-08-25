#pragma once

#include "script_instance.h"

namespace fuse {
    class scrolling_ground : public script_instance {
        FUSE_INLINE void on_start() override {
            width = get_asset<texture_asset>("ground")->instance.width;
        }

        FUSE_INLINE void on_update(float dt) override {
            auto &tr = get_component<ecs::transform_component>();

            if(tr.translate.x + width < 380) {
                tr.translate.x = 0;
            }
        }

    private:
        float width;
    };
}