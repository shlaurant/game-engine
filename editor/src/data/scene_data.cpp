#include "scene_data.h"
#include "common.h"

namespace editor {
    /// Guarantees basic exception safety
    /// \param path
    void scene_data::save(const std::filesystem::path &path) const {
        YAML::Emitter em;
        em << YAML::BeginMap << YAML::Key << "entities";
        em << YAML::BeginSeq;
        for (const auto &e: _entities) {
            e.serialize(em);
        }
        em << YAML::EndSeq;
        em << YAML::EndMap;

        std::ofstream filepath(path);
        filepath << em.c_str();
    }

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

    void scene_data::add_entity() {
        _entities.push_back(entity_data());
    }
    void scene_data::delete_entity(const entity_data &ett) {
        auto it = std::find_if(_entities.begin(), _entities.end(),
                               [ett](const entity_data &e) -> bool {
                                   return e == ett;
                               });
        if (it != _entities.end()) {
            _entities.erase(it);
        }
    }

    void scene_data::change_entity(entity_data e) {
        auto it = find(_entities.begin(), _entities.end(), e);
        if (it != _entities.end()) *it = std::move(e);
    }
}