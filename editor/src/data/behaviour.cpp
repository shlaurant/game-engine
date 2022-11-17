#include "behaviour.h"

namespace editor {

    void script_data::serialize(YAML::Emitter &em) const {
        em << YAML::Key << "script_component";
        em << YAML::BeginMap;
        em << YAML::Key << "name" << YAML::Value << name;
        em << YAML::EndMap;
    }

    void script_data::try_deserialize(const YAML::Node &node) {
        if (node["script_component"]) {
            deserialize(node["script_component"]);
        }
    }

    void script_data::deserialize(const YAML::Node &node) {
        name = node["name"].as<std::string>();
    }
}