#pragma once

#include <string>
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace editor {
    class entity_data {
    public:
        entity_data(const YAML::Node &);
        std::string name() const;

    private:
        std::string _name;
    };

    class scene_data {
    public:
        void load(const std::filesystem::path &);
        std::vector<entity_data> entities() const;

    private:
        std::vector<entity_data> _entities;
    };
}