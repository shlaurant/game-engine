#include "scene.h"

namespace editor::gui {
    scene_window::scene_window() {
        _load_dialog.SetTitle("Load");
        _load_dialog.SetTypeFilters({".yaml"});
    }

    void scene_window::show() {
        ImGui::Begin("Scene", nullptr,
                     ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar);

        ImGui::BeginMenuBar();
        if (ImGui::BeginMenu("Menu")) {
            if (ImGui::MenuItem("Load")) {
                _load_dialog.Open();
            }
            if (ImGui::MenuItem("Save")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();

        ImGui::Text("%s", _path.string().c_str());
        for (const auto &e: _data.entities()) {
            ImGui::Text("%s", e.name().c_str());
        }

        if (_loaded) {
            if(ImGui::Button("Add new entity")){
                _data.AddEntity();
            }
        }

        ImGui::End();

        _load_dialog.Display();
        if (_load_dialog.HasSelected()) {
            _path = _load_dialog.GetSelected();
            try {
                _data.load(_path);
                _loaded = true;
            } catch (...) {
                //TODO: show error gui
            }
            _load_dialog.ClearSelected();
        }
    }
}