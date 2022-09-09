#include "data.h"
#include "common/log.h"
#include "event/events.h"

namespace editor {

    scene_data::scene_data(dispatcher &disp) : _disp(disp) {
        auto p = std::shared_ptr<listener>(this);
        disp.add_listener<entity_change_event>(p);
    }

    void scene_data::on_event(std::shared_ptr<event> e) {
        auto p = std::static_pointer_cast<entity_change_event>(e);
        auto it = find(_entities.begin(), _entities.end(), p->new_data);
        if(it != _entities.end()) *it = p->new_data;
    }

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

        log_info("saved " + path.string());
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

        log_info("loaded " + path.string());
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
                                   return e.uuid() == ett.uuid();
                               });
        if (it != _entities.end()) {
            _entities.erase(it);
        }
    }

    /// \throw std::domain_error when the node is malformed
    /// \param ett_node
    entity_data::entity_data(const YAML::Node &ett_node) {
        if (auto i = ett_node["info_component"]) {
            info.uuid = i["uuid"].as<size_t>();
            info.name = i["name"].as<std::string>();
            info.tag = i["tag"].as<std::string>();
        } else {
            throw std::domain_error("There info_component for a entity");
        }
    }

    void entity_data::serialize(YAML::Emitter &em) const {
        em << YAML::BeginMap;
        info.serialize(em);
        em << YAML::EndMap;
    }

    std::string entity_data::name() const {
        return info.name;
    }

    size_t entity_data::uuid() const {
        return info.uuid;
    }

    void info_data::serialize(YAML::Emitter &em) const {
        em << YAML::Key << "info_component";
        em << YAML::BeginMap;
        em << YAML::Key << "uuid" << YAML::Value << uuid;
        em << YAML::Key << "name" << YAML::Value << name;
        em << YAML::Key << "tag" << YAML::Value << tag;
        em << YAML::EndMap;
    }
}