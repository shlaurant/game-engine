#include "common.h"

namespace editor::gui {
    std::string get_label(const std::string &str, int salt) {
        return std::string(str + "##" + std::to_string(salt));
    }

    log_window &get_log_window() {
        static log_window log_window;
        return log_window;
    }


    void log(const std::string &str) {
        get_log_window().log(str);
    }
}