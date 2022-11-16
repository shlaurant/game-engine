#include "graphics.h"

namespace editor {

    void sprite_data::serialize(YAML::Emitter &em) const {
        em << YAML::Key << "sprite_component";
        em << YAML::BeginMap;
        em << YAML::Key << "sprite" << YAML::Value << sprite;
        em << YAML::Key << "flip" << YAML::Value << flip;
        em << YAML::EndMap;
    }

    void sprite_data::try_deserialize(const YAML::Node &node) {
        if (node["sprite_component"]) {
            deserialize(node["sprite_component"]);
        }
    }

    void sprite_data::deserialize(const YAML::Node &node) {
        sprite = node["sprite"].as<fuse::uuid64>();
        flip = node["flip"].as<int>();
    }

    void animation_data::serialize(YAML::Emitter &em) const {
        em << YAML::Key << "animation_component";
        em << YAML::BeginMap;
        em << YAML::Key << "animation" << YAML::Value << animation;
        em << YAML::Key << "flip" << YAML::Value << flip;
        em << YAML::EndMap;
    }

    void animation_data::try_deserialize(const YAML::Node &node) {
        if(node["animation_component"]){
            deserialize(node["animation_component"]);
        }
    }

    void animation_data::deserialize(const YAML::Node &node) {
        animation = node["animation"].as<fuse::uuid64>();
        flip = node["flip"].as<int>();
    }

    void text_data::serialize(YAML::Emitter &em) const {
        em << YAML::Key << "text_component";
        em << YAML::BeginMap;
        em << YAML::Key << "R" << YAML::Value << (int) color.r;
        em << YAML::Key << "G" << YAML::Value << (int) color.g;
        em << YAML::Key << "B" << YAML::Value << (int) color.b;
        em << YAML::Key << "A" << YAML::Value << (int) color.a;
        em << YAML::Key << "text" << YAML::Value << text;
        em << YAML::Key << "font" << YAML::Value << font;
        em << YAML::Key << "flip" << YAML::Value << flip;
        em << YAML::EndMap;
    }

    void text_data::try_deserialize(const YAML::Node &node) {
        if(node["text_component"]){
            deserialize(node["text_component"]);
        }
    }

    void text_data::deserialize(const YAML::Node &node) {
        font = node["font"].as<fuse::uuid64>();
        text = node["text"].as<std::string>();
        flip = node["flip"].as<int>();
        color.r = node["R"].as<int>();
        color.g = node["G"].as<int>();
        color.b = node["B"].as<int>();
        color.a = node["A"].as<int>();
    }
}