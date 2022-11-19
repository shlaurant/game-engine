#pragma once

#include "scripts/script_instance.h"
#include "scripts/script_factory.h"

namespace fuse::ecs {
    struct script_component {
        FUSE_INLINE script_component(const script_component &) = default;
        FUSE_INLINE script_component() = default;

        template<typename T>
        FUSE_INLINE void bind() {
            instantiate = [](const script_props &props) {
                auto script = static_cast<script_instance *>(new T());
                script->init(props);
                return script;
            };
        }

        FUSE_INLINE void bind(const std::string &str) {
            instantiate = [str](const script_props &props) {
                auto script = script_factory::instance()->constructor(str)();
                script->init(props);
                return script;
            };
        }

        script_instance *instance = nullptr;
        script_instantiator instantiate;
        std::string name;
    };
}