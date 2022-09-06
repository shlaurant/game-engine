#include "scene.h"

namespace editor::gui {
    scene_window::scene_window() {
        _save_dialog.SetTitle("Load");
        _load_dialog.SetTitle("Load");
        _save_dialog.SetTypeFilters({".yaml"});
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
            if (ImGui::MenuItem("Save")) {
                _save_dialog.Open();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();

        ImGui::Text("%s", _path.string().c_str());
        for (const auto &e: _data.entities()) {
            ImGui::Text("%s", e.name().c_str());
        }

        if (_loaded) {
            if(ImGui::Button("Add new entity")){
                _data.add_entity();
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

        _save_dialog.Display();
        if(_save_dialog.HasSelected()){
            auto save_path = _save_dialog.GetSelected();
            _data.save(save_path);
        }
    }
}