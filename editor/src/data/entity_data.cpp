#include "entity_data.h"
#include "info_data.h"

namespace editor {

    /// \throw std::domain_error when the node is malformed
    /// \param ett_node
    entity_data::entity_data(const YAML::Node &ett_node) {
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