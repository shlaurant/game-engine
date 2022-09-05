#include "scene.h"

namespace editor::gui {
    scene_window::scene_window() {
        load_dialog.SetTitle("Load");
        load_dialog.SetTypeFilters({".yaml"});
    }

    void scene_window::show() {
        ImGui::Begin("Scene", nullptr,
                     ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar);

        ImGui::BeginMenuBar();
        if (ImGui::BeginMenu("Menu")) {
            if (ImGui::MenuItem("Load")) {
                load_dialog.Open();
            }
            if (ImGui::MenuItem("Save")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();

        ImGui::Text("%s", path.string().c_str());

        ImGui::End();

        load_dialog.Display();
        if (load_dialog.HasSelected()) {
            path = load_dialog.GetSelected();
            load_dialog.ClearSelected();
        }
    }
}