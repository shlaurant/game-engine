#pragma once

#include <string>
#include "log_window.h"

namespace editor::gui {
    std::string get_label(const std::string &, int);

    log_window &get_log_window();

    void log(const std::string &);
}