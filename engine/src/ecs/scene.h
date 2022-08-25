#pragma once

#include <scripts/player_controller.h>
#include "assets/registry.h"
#include "ecs/systems/sprite_renderer_system.h"
#include "ecs/systems/text_renderer_system.h"
#include "ecs/systems/frame_animation_system.h"
#include "ecs/systems/tilemap_renderer_system.h"
#include "ecs/systems/rigidbody_system.h"
#include "ecs/systems/collision_system.h"
#include "ecs/systems/script_system.h"

namespace fuse::ecs {
    class scene {
    public:
        FUSE_INLINE scene(SDL_Renderer *rd) : _renderer(rd) {
            register_system<ecs::sprite_renderer_system>();
            register_system<ecs::text_renderer_system>();
            register_system<ecs::frame_animation_system>();
            register_system<ecs::tilemap_renderer_system>();
            register_system<ecs::rigidbody_system>();
            register_system<ecs::collision_system>();
            register_system<ecs::script_system>();
        }

        FUSE_INLINE ~scene() {
            for (auto &s: _systems) { FUSE_DELETE(s); }
            _registry.clear();
            _systems.clear();
        }

        FUSE_INLINE ecs::entity add_entity(const std::string &name) {
            auto entity = ecs::entity(&_registry);
            entity.add_component<info_component>().name = name;
            entity.add_component<transform_component>();
            return entity;
        }

        FUSE_INLINE void update(float dt) {
            for (auto &sys: _systems) { sys->update(dt); }
        }

        FUSE_INLINE void start() {
            auto sp1 = _assets.load_texture("assets/obj1.png", "", _renderer);
            auto sp2 = _assets.load_texture("assets/obj2.png", "", _renderer);

            auto e1 = add_entity("e1");
            e1.add_component<script_component>().bind<player_controller>();
            e1.get_component<transform_component>().translate.x = 500;
            e1.add_component<sprite_component>().sprite = sp1->id;
            e1.add_component<collider_component>();

            auto e2 = add_entity("e2");
            e2.add_component<sprite_component>().sprite = sp2->id;
            e2.add_component<collider_component>();

            for (auto &sys: _systems) { sys->start(); }
        }

        template<typename T>
        FUSE_INLINE void register_system() {
            FUSE_STATIC_ASSERT(std::is_base_of<ecs::system, T>::value);
            auto new_system = new T();
            new_system->prepare(&_registry, _renderer, &_assets);
            _systems.push_back(new_system);
        }

    private:
        std::vector<ecs::system *> _systems;
        SDL_Renderer *_renderer = nullptr;
        ecs::registry _registry;
        asset_registry _assets;
    };
}