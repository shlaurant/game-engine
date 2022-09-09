#pragma once

#include <imgui.h>
#include <imfilebrowser.h>
#include "../data.h"
#include "../event/core.h"

namespace editor::gui {
    class scene_window {
    public:
        explicit scene_window(scene_data &, dispatcher &);
        void show();

    private:
        scene_data &_data;
        dispatcher &_disp;

        bool _loaded = false;
        ImGui::FileBrowser _load_dialog;
        ImGui::FileBrowser _save_dialog;
        std::filesystem::path _path;
        int _selection = -1;
    };
}