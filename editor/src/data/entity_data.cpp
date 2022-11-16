#include "entity_data.h"
#include "common.h"
#include "physics.h"

namespace editor {
    entity_data::entity_data() {
        auto info = std::make_shared<info_data>();
        data_map[fuse::type_id<info_data>()] = info;
    }

    /// \throw std::domain_error when the node is malformed
    /// \param ett_node
    entity_data::entity_data(const YAML::Node &ett_node) {
        init_comps(ett_node);
        for (auto &p: data_map) {
            p.second->try_deserialize(ett_node);
        }
    }

    void entity_data::serialize(YAML::Emitter &em) const {
        em << YAML::BeginMap;
        for (auto &p: data_map) {
            if (p.second != nullptr) p.second->serialize(em);
        }
        em << YAML::EndMap;
    }

    bool entity_data::operator==(const entity_data &rhs) const {
        return rhs.comp_data<info_data>()->uuid == comp_data<info_data>()->uuid;
    }

    void entity_data::init_comps(const YAML::Node &ett_node) {
        auto info = std::make_shared<info_data>();
        data_map[fuse::type_id<info_data>()] = info;

        if (ett_node["transform_component"]) {
            add_comp<transform_data>();
        }

        if (ett_node["rigidbody_component"]) {
            add_comp<rigidbody_data>();
        }

        if (ett_node["collider_component"]) {
            add_comp<collider_data>();
        }
    }
}