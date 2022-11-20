#pragma once

#include "gui/gui.h"

namespace editor::gui {
    class asset_window : public window {
    public:
        asset_window();

        void show() override;

    private:
        void show_add_menu();

        std::vector<std::function<void()>> _tabs;
    };
}

