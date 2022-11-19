#pragma once

#include "script_instance.h"

namespace fuse {
    class script_factory {
    public:
        static script_factory *instance();

        template<typename T>
        void register_script(const std::string &str) {
            static_assert(std::is_base_of<script_instance, T>::value);
            constructors[str] = []() { return new T(); };
        }

        std::function<script_instance *()> constructor(const std::string &);

    private:
        std::unordered_map<std::string, std::function<script_instance *()>> constructors;
    };
}

#define REGISTER_SCRIPT(script) fuse::script_factory::instance()->register_script<script>(#script);