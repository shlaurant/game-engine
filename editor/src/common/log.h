#pragma once

#include <string>
#include <iostream>

namespace editor {
    inline void log_info(const std::string &str) {
        std::cout << str << std::endl;
    }

    inline void log_err(const std::string &str) {
        std::cerr << str << std::endl;
    }
};