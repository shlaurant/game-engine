#include <imgui.h>

#include <utility>
#include "data/physics.h"
#include "data/common.h"
#include "entity_window.h"
#include "tabs.h"

namespace editor::gui {
    entity_window::entity_window() {
        _tabs.push_back(tab::information);
        _tabs.push_back(tab::transform);
        _tabs.push_back(tab::rigidbody);
        _tabs.push_back(tab::collider);
    }

    void entity_window::show() {
        if (!_is_open) return;

        ImGui::Begin("Entity");
        if (auto data = _entity_data.lock()) {
            for (auto &e: _tabs) e(*data);
            add_comp_popup(*data);

        } else {
            ImGui::Text("No scene loaded");
        }
        ImGui::End();
    }

    void entity_window::load_entity(std::weak_ptr<entity_data> p) {
        _entity_data = std::move(p);
    }

    void entity_window::add_comp_popup(entity_data &data) {
        static int selected_comp = -1;
        const char *comps[] = {"transform", "rigidbody", "collider"};

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
                data.add_comp<transform_data>();
                break;
            case 1:
                data.add_comp<rigidbody_data>();
                break;
            case 2:
                data.add_comp<collider_data>();
                break;
            default:
                //do nothing
                break;
        }
    }
}