#pragma once

#include <string>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include "event/core.h"
#include "entity_data.h"

namespace editor {
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