#include "info_data.h"

namespace editor {
    void info_data::serialize(YAML::Emitter &em) const {
        em << YAML::Key << "info_component";
        em << YAML::BeginMap;
        em << YAML::Key << "uuid" << YAML::Value << uuid;
        em << YAML::Key << "name" << YAML::Value << name;
        em << YAML::Key << "tag" << YAML::Value << tag;
        em << YAML::EndMap;
    }

    void info_data::try_deserialize(const YAML::Node &node) {
        if (node["info_component"]) {
            deserialize(node["info_component"]);
        } else {
            throw std::domain_error("no info_component for a entity");
        }
    }

    void info_data::deserialize(const YAML::Node &node) {
        uuid = node["uuid"].as<size_t>();
        name = node["name"].as<std::string>();
        tag = node["tag"].as<std::string>();
    }
}
