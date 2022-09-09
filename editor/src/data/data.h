#pragma once

#include <string>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include "event/core.h"

namespace editor {
    struct info_data {
        size_t uuid = fuse::generate_uuid();
        std::string name = "new entity";
        std::string tag = "default";

        void serialize(YAML::Emitter &) const;
    };

    struct entity_data {
        entity_data() = default;
        explicit entity_data(const YAML::Node &);

        void serialize(YAML::Emitter &) const;
        std::string name() const;
        size_t uuid() const;

        bool operator==(const entity_data &rhs) const {
            return rhs.info.uuid == info.uuid;
        }

        info_data info;
    };

    class scene_data {
    public:
        void save(const std::filesystem::path &) const;
        void load(const std::filesystem::path &);
        std::vector<entity_data> entities() const;
        void add_entity();
        void delete_entity(const entity_data &);
        void change_entity(entity_data);

    private:
        std::vector<entity_data> _entities;
    };
}