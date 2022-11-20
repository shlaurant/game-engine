#pragma once

#include <yaml-cpp/yaml.h>
#include "core/typeid.h"

namespace editor {
    struct asset {
        uint32_t type_id;

        fuse::uuid64 asset_id = fuse::generate_uuid();
        std::string name = "new asset";

        static std::shared_ptr<asset> create(const YAML::Node &);

        virtual void serialize(YAML::Emitter &) const = 0;

    protected:
        void serialize_base(YAML::Emitter &) const;

    private:
        template<typename T>
        void deserialize_base(const YAML::Node &data) {
            type_id = fuse::type_id<T>();
            asset_id = data["id"].as<fuse::uuid64>();
            name = data["name"].as<std::string>();
        }
    };

    struct texture : public asset {
        std::string source;

        void serialize(YAML::Emitter &emitter) const override;
    };

    struct animation : public asset {
        int speed;
        std::vector<fuse::uuid64> frames;

        void serialize(YAML::Emitter &emitter) const override;
    };

    struct audio : public asset {
        std::string source;

        void serialize(YAML::Emitter &emitter) const override;
    };

    struct font : public asset {
        int size;
        std::string source;

        void serialize(YAML::Emitter &emitter) const override;
    };
}