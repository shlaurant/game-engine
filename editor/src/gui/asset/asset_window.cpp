#include <imgui.h>
#include "asset_window.h"
#include "data/scene_data.h"
#include "asset_tabs.h"

namespace editor::gui {
    asset_window::asset_window() {
        _is_open = true;

        _tabs.emplace_back(asset_tab::font);
        _tabs.emplace_back(asset_tab::texture);
        _tabs.emplace_back(asset_tab::animation);
        _tabs.emplace_back(asset_tab::audio);
    }

    void asset_window::show() {
        if (!_is_open) return;

        ImGui::Begin("asset");

        if (scene_data::instance()->is_loaded()) {
            for (const auto &e: _tabs) e();
            show_add_menu();
        } else {
            ImGui::Text("no scene loaded");
        }

        ImGui::End();
    }

    void asset_window::show_add_menu() {
        int selected_comp = -1;
        const char *comps[] = {"texture", "animation", "font", "audio"};

        if (ImGui::Button("Add")) {
            ImGui::OpenPopup("popup");
        }

        if (ImGui::BeginPopup("popup")) {
            for (auto i = 0; i < IM_ARRAYSIZE(comps); ++i) {
                if (ImGui::Selectable(comps[i])) selected_comp = i;
            }
            ImGui::EndPopup();
        }

        switch (selected_comp) {
            case 0:
                scene_data::instance()->add_asset<texture>();
                break;
            case 1:
                scene_data::instance()->add_asset<animation>();
                break;
            case 2:
                scene_data::instance()->add_asset<font>();
                break;
            case 3:
                scene_data::instance()->add_asset<audio>();
                break;
            default:
                //do nothing
                break;
        }
    }
}
