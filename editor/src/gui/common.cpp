#include "common.h"

namespace editor::gui {
    std::string get_label(const std::string &str, int salt) {
        return std::string(str + "##" + std::to_string(salt));
    }
}