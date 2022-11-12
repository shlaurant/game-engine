#include "entity/entity_window.h"
#include "gui.h"
#include "scene_window.h"

namespace editor::gui {
    void gui::show() {
        for (auto &e: _windows) {
            e.second->show();
        }
    }

    gui::gui() {
        auto scene = std::make_shared<scene_window>();
        auto entity= std::make_shared<entity_window>();
        add_window(scene);
        add_window(entity);
    }
}