#pragma once

#include "entity_data.h"

namespace editor {
    struct info_data : public component_data {
        size_t uuid = fuse::generate_uuid();
        std::string name = "new entity";
        std::string tag = "default";

        void serialize(YAML::Emitter &) const override;
        void try_deserialize(const YAML::Node &node) override;
        void deserialize(const YAML::Node &) override;
    };
}
