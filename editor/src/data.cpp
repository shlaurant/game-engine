#include "data.h"

namespace editor {
    /// Guarantees basic exception safety
    /// \param path
    void scene_data::load(const std::filesystem::path &path) {
        auto root = YAML::LoadFile(path.string());

        if (auto ett_node = root["entities"]) {
            std::vector<entity_data> new_entities;
            for (const auto &ett: ett_node) {
                new_entities.push_back(entity_data(ett));
            }
            _entities = new_entities;
        }
    }

    std::vector<entity_data> scene_data::entities() const {
        return _entities;
    }

    void scene_data::AddEntity() {
        _entities.push_back(entity_data());
    }

    entity_data::entity_data(const YAML::Node &ett_node) {
        if (auto info = ett_node["info_component"]) {
            _info.uuid = info["uuid"].as<size_t>();
            _info.name = info["name"].as<std::string>();
            _info.tag = info["tag"].as<std::string>();
        }
    }

    std::string entity_data::name() const {
        return _info.name;
    }
}