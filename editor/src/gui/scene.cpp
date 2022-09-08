#include "scene.h"
#include "common.h"

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

        if (_loaded) {
            ImGui::Text("%s", _path.string().c_str());
            ImGui::Indent(15);

            const auto &&entities = _data.entities();
            for (auto i = 0; i < entities.size(); ++i) {
                auto label = entities[i].name();
                label += "##" + std::to_string(i);
                if (ImGui::Selectable(label.c_str(), i == _selection)) {
                    _selection = i;
                }
                if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
                {
                    if (ImGui::Button("Delete")) {
                        _data.delete_entity(entities[i]);
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }

            if (ImGui::Button("Add new entity")) {
                _data.add_entity();
            }
        }


        ImGui::End();

        _load_dialog.Display();
        if (_load_dialog.HasSelected()) {
            _path = _load_dialog.GetSelected();
            log(_path.string() + " selected in load dialog");
            try {
                _data.load(_path);
                _loaded = true;
                _selection = -1;
            } catch (...) {
                //TODO: show error gui
            }
            _load_dialog.ClearSelected();
        }

        _save_dialog.Display();
        if (_save_dialog.HasSelected()) {
            auto save_path = _save_dialog.GetSelected();
            _data.save(save_path);
            _save_dialog.ClearSelected();
        }
    }
}