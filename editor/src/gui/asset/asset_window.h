#pragma once

#include "gui/gui.h"

namespace editor::gui {
    class asset_window : public window {
    public:
        asset_window();

        void show() override;

    private:
        std::vector<std::function<void()>> _tabs;
    };
}

