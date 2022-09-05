#pragma once

#include <imgui.h>
#include <imfilebrowser.h>

namespace editor::gui {
    class scene_window {
    public:
        scene_window();
        void show();

    private:
        ImGui::FileBrowser load_dialog;
        std::filesystem::path path;
    };
}