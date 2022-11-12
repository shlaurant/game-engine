#pragma once

#include <imgui.h>
#include <imfilebrowser.h>
#include "data/scene_data.h"
#include "event/core.h"
#include "gui.h"

namespace editor::gui {
    class scene_window: public window {
    public:
        scene_window();

        void show() override;

    private:
        bool _create_new = false;
        bool _loaded = false;
        ImGui::FileBrowser _load_dialog;
        ImGui::FileBrowser _save_dialog;
        std::filesystem::path _path;
        int _selection = -1;
    };
}