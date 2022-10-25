#pragma once

#include <memory>
#include "scene_window.h"
#include "entity/entity_window.h"
#include "event/core.h"

namespace editor::gui {
    class gui {
    public:
        gui(std::shared_ptr<dispatcher>, std::shared_ptr<scene_data>);

        void show();
    private:
        std::shared_ptr<scene_window> _p_scene_window;
        std::shared_ptr<entity_window> _p_entity_window;
    };
}