#include "entity_data.h"
#include "info_data.h"

namespace editor {
    entity_data::entity_data() {
        auto info = std::make_shared<info_data>();
        data_map[fuse::type_id<info_data>()] = info;
    }

    /// \throw std::domain_error when the node is malformed
    /// \param ett_node
    entity_data::entity_data(const YAML::Node &ett_node) {
        auto info = std::make_shared<info_data>();
        data_map[fuse::type_id<info_data>()] = info;
        for (auto &p: data_map) {
            p.second->try_deserialize(ett_node);
        }
    }

    void entity_data::serialize(YAML::Emitter &em) const {
        em << YAML::BeginMap;
        for (auto &p: data_map) {
            p.second->serialize(em);
        }
        em << YAML::EndMap;
    }

    bool entity_data::operator==(const entity_data &rhs) const {
        return rhs.comp_data<info_data>()->uuid == comp_data<info_data>()->uuid;
    }
}