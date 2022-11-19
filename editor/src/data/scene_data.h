#pragma once

#include <string>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include "event/core.h"
#include "entity_data.h"
#include "assets.h"

namespace editor {
    class scene_data {
    public:
        static scene_data *instance() {
            static scene_data i;
            return &i;
        }

        void save(const std::filesystem::path &) const;
        void load(const std::filesystem::path &);
        bool is_loaded();

        std::vector<std::weak_ptr<entity_data>> entities();
        void add_entity();
        void delete_entity(const entity_data &);

        template<typename T>
        std::vector<std::weak_ptr<T>> assets() {
            static_assert(std::is_base_of<asset, T>::value);
            std::vector<std::weak_ptr<T>> ret;
            auto v = _assets[fuse::type_id<T>()];
            for (auto e: v) {
                ret.emplace_back(std::static_pointer_cast<T>(e));
            }

            return ret;
        }

    private:
        scene_data() = default;

        bool _is_loaded = false;

        std::vector<std::shared_ptr<entity_data>> _entities;
        std::unordered_map<fuse::uuid64, std::vector<std::shared_ptr<asset>>> _assets;
    };
}