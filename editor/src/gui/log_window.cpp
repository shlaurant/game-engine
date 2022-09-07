#include "log_window.h"
#include <imgui.h>

namespace editor::gui {
    void log_window::show() const {
        ImGui::Begin("Log");
        for (const auto &e: _logs) {
            ImGui::Text("%s", e.c_str());
        }
        ImGui::End();
    }

    void log_window::log(const std::string &str) {
        _logs.push_back(str);
    }
}