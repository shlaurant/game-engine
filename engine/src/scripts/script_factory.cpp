#include "script_factory.h"

namespace fuse {
    script_factory *script_factory::instance() {
        static script_factory sf;
        return &sf;
    }

    std::function<script_instance *()>
    script_factory::constructor(const std::string &str) {
        return constructors[str];
    }
}
