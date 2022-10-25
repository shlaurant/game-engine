#pragma once

#include "entity_window.h"

namespace editor::gui::tab {
    const static int BuffSize = 24;
    static char name[BuffSize] = "";
    static char tag[BuffSize] = "";

    bool information(entity_data &);
}