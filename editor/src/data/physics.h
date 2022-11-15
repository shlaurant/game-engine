#pragma once

#include "entity_data.h"
#include "math/vec2.h"

namespace editor {
    struct rigidbody_data : public component_data {
        void serialize(YAML::Emitter &emitter) const override;
        void try_deserialize(const YAML::Node &node) override;
        void deserialize(const YAML::Node &node) override;

        bool disabled = false;
        float gravity_scale;
        fuse::vec2f velocity;
        fuse::vec2f force;
    };

    struct collider_data: public component_data {
        void serialize(YAML::Emitter &emitter) const override;
        void try_deserialize(const YAML::Node &node) override;
        void deserialize(const YAML::Node &node) override;

        float w;
        float h;
        bool disabled = false;
    };
}