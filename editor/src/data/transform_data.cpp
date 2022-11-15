#include "transform_data.h"
#include "helpers/yaml.h"

void editor::transform_data::serialize(YAML::Emitter &em) const {
    em << YAML::Key << "transform_component";
    em << YAML::BeginMap;
    em << YAML::Key << "translate" << YAML::Value << translate;
    em << YAML::Key << "rotation" << YAML::Value << rotation;
    em << YAML::Key << "scale" << YAML::Value << scale;
    em << YAML::EndMap;
}

void editor::transform_data::try_deserialize(const YAML::Node &node) {
    if(node["transform_component"]){
        deserialize(node["transform_component"]);
    }
}

void editor::transform_data::deserialize(const YAML::Node &node) {
    translate = node["translate"].as<fuse::vec2f>();
    rotation = node["rotation"].as<float>();
    scale = node["scale"].as<fuse::vec2f>();
}