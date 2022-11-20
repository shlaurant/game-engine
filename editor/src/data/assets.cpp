#include "assets.h"

namespace editor {
    std::shared_ptr<asset> asset::create(const YAML::Node &node) {
        if (auto data = node["texture"]) {
            auto ret = std::make_shared<texture>();

            ret->deserialize_base<texture>(data);
            ret->source = data["source"].as<std::string>();

            return ret;
        }

        if (auto data = node["animation"]) {
            auto ret = std::make_shared<animation>();

            ret->deserialize_base<animation>(data);
            ret->speed = data["speed"].as<int>();
            int i = 0;
            while (true) {
                auto temp = "frame" + std::to_string(i);
                if (!data[temp]) break;

                auto frame = data[temp].as<fuse::uuid64>();
                ret->frames[i] = frame;
                ++i;
            }

            return ret;
        }

        if (auto data = node["audio"]) {
            auto ret = std::make_shared<audio>();

            ret->deserialize_base<audio>(data);
            ret->source = data["source"].as<std::string>();

            return ret;
        }

        if (auto data = node["font"]) {
            auto ret = std::make_shared<font>();

            ret->deserialize_base<font>(data);
            ret->size = data["size"].as<int>();
            ret->source = data["source"].as<std::string>();

            return ret;
        }

        FUSE_ERROR("unexpected asset type")
        return nullptr;
    }

    void asset::serialize_base(YAML::Emitter &em) const {
        em << YAML::Key << "id" << YAML::Value << asset_id;
        em << YAML::Key << "name" << YAML::Value << name;
    }

    void texture::serialize(YAML::Emitter &em) const {
        em << YAML::BeginMap;
        em << YAML::Key << "texture";
        em << YAML::BeginMap;
        serialize_base(em);
        em << YAML::Key << "source" << YAML::Value << source;
        em << YAML::EndMap;
        em << YAML::EndMap;
    }

    void animation::serialize(YAML::Emitter &em) const {
        //TODO: impl
    }

    void audio::serialize(YAML::Emitter &em) const {
        em << YAML::BeginMap;
        em << YAML::Key << "audio";
        em << YAML::BeginMap;
        serialize_base(em);
        em << YAML::Key << "source" << YAML::Value << source;
        em << YAML::EndMap;
        em << YAML::EndMap;
    }

    void font::serialize(YAML::Emitter &em) const {
        em << YAML::BeginMap;
        em << YAML::Key << "font";
        em << YAML::BeginMap;
        serialize_base(em);
        em << YAML::Key << "source" << YAML::Value << source;
        em << YAML::Key << "size" << YAML::Value << size;
        em << YAML::EndMap;
        em << YAML::EndMap;
    }
};