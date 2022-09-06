#pragma once

#include <string>
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace editor {
    struct info_data {
        size_t uuid = fuse::generate_uuid();
        std::string name = "new entity";
        std::string tag = "default";
    };

    class entity_data {
    public:
        entity_data() = default;
        explicit entity_data(const YAML::Node &);
        std::string name() const;

    private:
        info_data _info;
    };

    class scene_data {
    public:
        void load(const std::filesystem::path &);
        std::vector<entity_data> entities() const;
        void AddEntity();

    private:
        std::vector<entity_data> _entities;
    };
}