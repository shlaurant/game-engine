#pragma once

#include <string>
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace editor {
    struct info_data {
        size_t uuid = fuse::generate_uuid();
        std::string name = "new entity";
        std::string tag = "default";

        void serialize(YAML::Emitter &) const;
    };

    class entity_data {
    public:
        entity_data() = default;
        explicit entity_data(const YAML::Node &);

        void serialize(YAML::Emitter &) const;
        std::string name() const;

    private:
        info_data _info;
    };

    class scene_data {
    public:
        void save(const std::filesystem::path &) const;
        void load(const std::filesystem::path &);
        std::vector<entity_data> entities() const;
        void add_entity();

    private:
        std::vector<entity_data> _entities;
    };
}