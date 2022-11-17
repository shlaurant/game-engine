#pragma once

#include <string>
#include "entity_data.h"

namespace editor {
    struct script_data : public component_data {
        std::string name;

        void serialize(YAML::Emitter &emitter) const override;
        void try_deserialize(const YAML::Node &node) override;
        void deserialize(const YAML::Node &node) override;
    };
}