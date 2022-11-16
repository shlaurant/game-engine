#include <imgui.h>
#include "data/physics.h"
#include "data/common.h"
#include "entity_window.h"
#include "tabs.h"

namespace editor::gui {
    entity_window::entity_window() {
        _tabs.push_back(tab::information);
        _tabs.push_back(tab::transform);
        _tabs.push_back(tab::rigidbody);
    }

    void entity_window::show() {
        if (!_is_open) return;

        ImGui::Begin("Entity");
        if (_entity_loaded) {
            bool is_changed = false;


            for (auto &e: _tabs) {
                is_changed = e(_entity_data);
            }

            add_comp_popup(is_changed);

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

    void entity_window::add_comp_popup(bool &is_changed) {
        static int selected_comp = -1;
        const char *comps[] = {"transform", "rigidbody"};

        if (ImGui::Button("Add")) {
            ImGui::OpenPopup("popup");
            ImGui::TextUnformatted(
                    selected_comp == -1 ? "None" : comps[selected_comp]);
        }

        if (ImGui::BeginPopup("popup")) {
            for (auto i = 0; i < IM_ARRAYSIZE(comps); ++i) {
                if (ImGui::Selectable(comps[i])) selected_comp = i;
            }
            ImGui::EndPopup();
        }

        switch (selected_comp) {
            case 0:
                _entity_data.add_comp<transform_data>();
                is_changed = true;
                break;
            case 1:
                _entity_data.add_comp<rigidbody_data>();
                is_changed = true;
                break;
            default:
                //do nothing
                break;
        }
    }
}