#include "scene_data.h"
#include "common.h"

namespace editor {
    bool scene_data::is_loaded() {
        return _is_loaded;
    }

    /// Guarantees basic exception safety
    /// \param path
    void scene_data::save(const std::filesystem::path &path) const {
        YAML::Emitter em;
        em << YAML::BeginMap << YAML::Key << "entities";
        em << YAML::BeginSeq;
        for (const auto &e: _entities) {
            e->serialize(em);
        }
        em << YAML::EndSeq;

        em << YAML::Key << "assets" << YAML::Value << YAML::BeginSeq;

        for (const auto &e: _assets) {
            for (auto &p: e.second) {
                p->serialize(em);
            }
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
            std::vector<std::shared_ptr<entity_data>> new_entities;
            for (auto ett: ett_node) {
                auto p = std::make_shared<entity_data>(ett);
                new_entities.push_back(p);
            }
            _entities = new_entities;
        }

        if (auto asset_nodes = root["assets"]) {
            std::unordered_map<uint32_t, std::vector<std::shared_ptr<asset>>> tmp;
            for (auto e: asset_nodes) {
                auto p = asset::create(e);
                tmp[p->type_id].emplace_back(p);
            }
            _assets = tmp;
        }

        _is_loaded = true;
    }

    std::vector<std::weak_ptr<entity_data>> scene_data::entities() {
        std::vector<std::weak_ptr<entity_data>> ret;
        for (auto &e: _entities) ret.push_back(e);
        return ret;
    }

    void scene_data::add_entity() {
        _entities.push_back(std::make_shared<entity_data>());
    }

    void scene_data::delete_entity(const entity_data &ett) {
        auto it = std::find_if(_entities.begin(), _entities.end(),
                               [ett](const std::shared_ptr<entity_data> &e) -> bool {
                                   return *e == ett;
                               });
        if (it != _entities.end()) {
            _entities.erase(it);
        }
    }
}