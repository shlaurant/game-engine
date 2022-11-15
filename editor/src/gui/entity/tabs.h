#pragma once

#include "entity_window.h"

namespace editor::gui::tab {
    const static int BuffSize = 24;

    bool information(entity_data &);
    bool transform(entity_data &);

    //helper method used as if it were macro
    bool input_text(const char *, char (&)[BuffSize] , ImGuiInputTextFlags_);
}