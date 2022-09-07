#pragma once

#include <string>

namespace editor::gui {
    class log_window {
    public:
        void show() const;
        void log(const std::string &);

    private:
        std::vector<std::string> _logs;
    };
}