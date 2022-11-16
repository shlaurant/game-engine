#pragma once

#include <yaml-cpp/yaml.h>
#include <core/typeid.h>

namespace editor {
    struct component_data {
        virtual ~component_data() = default;

        virtual void serialize(YAML::Emitter &) const = 0;

        virtual void try_deserialize(const YAML::Node &) = 0;

        virtual void deserialize(const YAML::Node &) = 0;
    };

    struct entity_data {
        entity_data();

        explicit entity_data(const YAML::Node &);

        void serialize(YAML::Emitter &) const;

        template<typename T>
        std::shared_ptr<T> comp_data() {
            return std::static_pointer_cast<T>(data_map[fuse::type_id<T>()]);
        };

        template<typename T>
        std::shared_ptr<const T> comp_data() const {
            return std::static_pointer_cast<const T>(
                    data_map.at(fuse::type_id<T>()));
        }

        template<typename T>
        void add_comp() {
            data_map[fuse::type_id<T>()] = std::make_shared<T>();
        }

        template<typename T>
        bool has_comp() {
            return data_map[fuse::type_id<T>()] != nullptr;
        }

        template<typename T>
        void delete_comp() {
            data_map.erase(fuse::type_id<T>());
        }

        bool operator==(const entity_data &) const;

        std::map<uint32_t, std::shared_ptr<component_data>> data_map;

    private:
        void init_comps(const YAML::Node &);
    };
}