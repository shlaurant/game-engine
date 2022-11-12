#include <imgui.h>
#include "entity_window.h"
#include "tabs.h"

namespace editor::gui {
    entity_window::entity_window() {
        _tabs.push_back(tab::information);
    }

    void entity_window::show() {
        if (!_is_open) return;

        ImGui::Begin("Entity");
        if (_entity_loaded) {
            bool is_changed = false;


            for (auto &e: _tabs) {
                is_changed = e(_entity_data);
            }

            if (is_changed) {
                scene_data::instance()->change_entity(_entity_data);
            }

        } else {
            ImGui::Text("No scene loaded");
        }
        ImGui::End();
    }

    void entity_window::load_entity(entity_data e) {
        _entity_data = std::move(e);
        _entity_loaded = true;
    }
}