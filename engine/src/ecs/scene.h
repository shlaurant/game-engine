#pragma once

#include "ecs/systems/sprite_renderer_system.h"
#include "assets/registry.h"

namespace fuse::ecs {
    class scene {
    public:
        FUSE_INLINE scene(SDL_Renderer *rd) : _renderer(rd) {
            register_system<ecs::sprite_renderer_system>();
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
            auto sprite = _assets.load_texture("assets/test.png", "test",
                                               _renderer);
            ecs::entity entity = add_entity("entity");
            entity.add_component<ecs::sprite_component>().sprite = sprite->id;

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