#include <imgui.h>
#include "asset_tabs.h"
#include "data/scene_data.h"

namespace editor::gui::asset_tab {
    void animation() {
        if (ImGui::TreeNode("animations")) {
            ImGui::Indent();
            auto animations = scene_data::instance()->assets<editor::animation>();
            for (auto &tmp: animations) {
                if (auto p = tmp.lock()) {
                    ImGui::Separator();
                    ImGui::Text("uuid");
                    ImGui::Text("name");
                }
            }
            ImGui::Unindent();
            ImGui::TreePop();
        }
    }

    void audio() {

    }

    void font() {

    }

    void texture() {

    }
}