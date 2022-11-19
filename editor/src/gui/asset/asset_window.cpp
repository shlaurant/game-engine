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
        } else {
            ImGui::Text("no scene loaded");
        }

        ImGui::End();
    }
}
