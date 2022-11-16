#include "physics.h"
#include "helpers/yaml.h"

namespace editor {
    void rigidbody_data::serialize(YAML::Emitter &em) const {
        em << YAML::Key << "rigidbody_component";
        em << YAML::BeginMap;
        em << YAML::Key << "gravity_scale" << YAML::Value << gravity_scale;
        em << YAML::Key << "velocity" << YAML::Value << velocity;
        em << YAML::Key << "disabled" << YAML::Value << disabled;
        em << YAML::Key << "force" << YAML::Value << force;
        em << YAML::EndMap;
    }

    void rigidbody_data::try_deserialize(const YAML::Node &node) {
        if (node["rigidbody_component"]) {
            deserialize(node["rigidbody_component"]);
        }
    }

    void rigidbody_data::deserialize(const YAML::Node &node) {
        disabled = node["disabled"].as<bool>();
        gravity_scale = node["gravity_scale"].as<float>();
        velocity = node["velocity"].as<fuse::vec2f>();
        force = node["force"].as<fuse::vec2f>();
    }

    void collider_data::serialize(YAML::Emitter &em) const {
        em << YAML::Key << "collider_component";
        em << YAML::BeginMap;
        em << YAML::Key << "width" << YAML::Value << w;
        em << YAML::Key << "height" << YAML::Value << h;
        em << YAML::Key << "disabled" << YAML::Value << disabled;
        em << YAML::EndMap;
    }

    void collider_data::try_deserialize(const YAML::Node &node) {
        if (node["collider_component"]) deserialize(node["collider_component"]);
    }

    void collider_data::deserialize(const YAML::Node &node) {
        w = node["width"].as<float>();
        h = node["height"].as<float>();
        disabled = node["disabled"].as<bool>();
    }
}