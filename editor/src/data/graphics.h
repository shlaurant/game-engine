#pragma once

#include "entity_data.h"

namespace editor {
    struct sprite_data : public component_data {
        fuse::uuid64 sprite;
        int flip;

        void serialize(YAML::Emitter &emitter) const override;
        void try_deserialize(const YAML::Node &node) override;
        void deserialize(const YAML::Node &node) override;
    };

    struct animation_data : public component_data {
        fuse::uuid64 animation;
        int flip;

        void serialize(YAML::Emitter &emitter) const override;
        void try_deserialize(const YAML::Node &node) override;
        void deserialize(const YAML::Node &node) override;
    };

    struct text_data : public component_data {
        fuse::uuid64 font;
        std::string text;
        int flip;
        SDL_Color color;

        void serialize(YAML::Emitter &emitter) const override;
        void try_deserialize(const YAML::Node &node) override;
        void deserialize(const YAML::Node &node) override;
    };

//    typedef enum
//    {
//        SDL_FLIP_NONE = 0x00000000,     /**< Do not flip */
//        SDL_FLIP_HORIZONTAL = 0x00000001,    /**< flip horizontally */
//        SDL_FLIP_VERTICAL = 0x00000002     /**< flip vertically */
//    } SDL_RendererFlip;
}