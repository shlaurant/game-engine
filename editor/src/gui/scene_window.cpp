#include "scene_window.h"
#include "entity/entity_window.h"
#include "data/common.h"

namespace editor::gui {
    scene_window::scene_window() {
        _is_open = true;
        _save_dialog.SetTitle("Save");
        _load_dialog.SetTitle("Load");
        _save_dialog.SetTypeFilters({".yaml"});
        _load_dialog.SetTypeFilters({".yaml"});
    }

    void scene_window::show() {
        ImGui::Begin("Scene", nullptr,
                     ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar);

        ImGui::BeginMenuBar();
        if (ImGui::BeginMenu("Menu")) {
            if (ImGui::MenuItem("New")) {
                _create_new = true;
            }
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
            show_entities();
        }

        ImGui::End();

        if (_create_new) {
            show_new();
        }

        show_load();

        show_save();
    }

    void scene_window::show_entities() {
        ImGui::Text("%s", _path.string().c_str());
        ImGui::Indent(15);

        const auto &&entities = scene_data::instance()->entities();
        for (auto i = 0; i < entities.size(); ++i) {
            auto label = entities[i].comp_data<info_data>()->name;
            label += "##" + std::to_string(i);
            if (ImGui::Selectable(label.c_str(), i == _selection)) {
                _selection = i;
                gui::instance()->p_window<entity_window>()->open();
                gui::instance()->p_window<entity_window>()->load_entity(
                        scene_data::instance()->entities()[i]);
            }
            if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
            {
                if (ImGui::Button("Delete")) {
                    scene_data::instance()->delete_entity(entities[i]);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        if (ImGui::Button("Add new entity")) {
            scene_data::instance()->add_entity();
        }
    }

    void scene_window::show_load() {
        _load_dialog.Display();
        if (_load_dialog.HasSelected()) {
            _path = _load_dialog.GetSelected();
            try {
                scene_data::instance()->load(_path);
                _loaded = true;
                _selection = -1;
            } catch (...) {
                //TODO: show error gui
            }
            _load_dialog.ClearSelected();
        }
    }

    void scene_window::show_save() {
        _save_dialog.Display();
        if (_save_dialog.HasSelected()) {
            auto save_path = _save_dialog.GetSelected();
            scene_data::instance()->save(save_path);
            _save_dialog.ClearSelected();
        }
    }

    void scene_window::show_new() {
        static const int buf_size = 24;
        static char file_name_buf[buf_size] = {0,};
        ImGui::Begin("New");
        ImGui::InputText("name", file_name_buf, buf_size);
        if (ImGui::Button("Create")) {
            std::ofstream file(std::string(file_name_buf) + ".yaml");
            file.close();
            _create_new = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            _create_new = false;
        }
        ImGui::End();
    }
}