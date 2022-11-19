#pragma once

#include <yaml-cpp/yaml.h>
#include "core/typeid.h"

namespace editor {
    struct asset {
        uint32_t type_id;

        fuse::uuid64 asset_id;
        std::string name;

        static std::shared_ptr<asset> create(const YAML::Node &);

    private:
        template<typename T>
        void serialize_base(const YAML::Node &data) {
            type_id = fuse::type_id<T>();
            asset_id = data["id"].as<fuse::uuid64>();
            name = data["name"].as<std::string>();
        }
    };

    struct texture : public asset {
        std::string source;
    };

    struct animation : public asset {
        int speed;
        std::vector<fuse::uuid64> frames;
    };

    struct audio : public asset {
        std::string source;
    };

    struct font : public asset {
        int size;
        std::string source;
    };
}