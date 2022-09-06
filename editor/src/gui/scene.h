#pragma once

#include <imgui.h>
#include <imfilebrowser.h>
#include "../data.h"

namespace editor::gui {
    class scene_window {
    public:
        scene_window();
        void show();

    private:
        bool _loaded = false;

        ImGui::FileBrowser _load_dialog;
        ImGui::FileBrowser _save_dialog;
        std::filesystem::path _path;
        scene_data _data;
    };
}