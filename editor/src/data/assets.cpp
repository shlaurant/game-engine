#include "assets.h"

namespace editor {
    std::shared_ptr<asset> asset::create(const YAML::Node &node) {
        if (auto data = node["texture"]) {
            auto ret = std::make_shared<texture>();

            ret->serialize_base<texture>(data);
            ret->source = data["source"].as<std::string>();

            return ret;
        }

        if (auto data = node["animation"]) {
            auto ret = std::make_shared<animation>();

            ret->serialize_base<animation>(data);
            ret->speed = data["speed"].as<int>();
            int i = 0;
            while(true){
                auto temp = "frame" + std::to_string(i);
                if(!data[temp]) break;

                auto frame = data[temp].as<fuse::uuid64>();
                ret->frames[i] = frame;
                ++i;
            }

            return ret;
        }

        if (auto data = node["audio"]) {
            auto ret = std::make_shared<audio>();

            ret->serialize_base<audio>(data);
            ret->source = data["source"].as<std::string>();

            return ret;
        }

        if (auto data= node["font"]) {
            auto ret = std::make_shared<font>();

            ret->serialize_base<font>(data);
            ret->size = data["size"].as<int>();
            ret->source = data["source"].as<std::string>();

            return ret;
        }

        FUSE_ERROR("unexpected asset type")
        return nullptr;
    }
};