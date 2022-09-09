#pragma once

#include <imgui.h>
#include <imfilebrowser.h>
#include "data/data.h"
#include "event/core.h"

namespace editor::gui {
    class scene_window {
    public:
        scene_window(std::shared_ptr<dispatcher>, std::shared_ptr<scene_data>);

        void show();

    private:
        std::shared_ptr<dispatcher> _disp;
        std::shared_ptr<scene_data> _data;

        bool _loaded = false;
        ImGui::FileBrowser _load_dialog;
        ImGui::FileBrowser _save_dialog;
        std::filesystem::path _path;
        int _selection = -1;
    };
}