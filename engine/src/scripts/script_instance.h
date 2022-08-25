#pragma once

#include "ecs/components/graphics.h"
#include "ecs/components/physics.h"
#include "ecs/components/common.h"
#include "ecs/entity.h"
#include "assets/registry.h"
#include "core/inputs.h"

namespace fuse {
    struct script_props {
        FUSE_INLINE script_props(ecs::entityid e, ecs::registry *r,
                                 asset_registry *a) : entity(e), registry(r),
                                                      assets(a) {}
        ecs::entityid entity = INVALID_ID;
        ecs::registry *registry = nullptr;
        asset_registry *assets = nullptr;
    };

    class script_instance {
    public:
        FUSE_INLINE virtual ~script_instance() = default;

        FUSE_INLINE void init(const script_props &props) {
            _registry = props.registry;
            _assets = props.assets;
            _entity = props.entity;
        }

        FUSE_INLINE virtual void on_collision(ecs::entity) {}
        FUSE_INLINE virtual void on_update(float) {}
        FUSE_INLINE virtual void on_destroy() {}
        FUSE_INLINE virtual void on_start() {}

    protected:
        template<typename T, typename... Args>
        FUSE_INLINE T &add_component(Args &&... args) {
            return _registry->add_component<T>(_entity,
                                               std::forward<Args>(args)...);
        }

        template<typename T>
        FUSE_INLINE T &get_component() {
            return _registry->get_component<T>(_entity);
        }

        template<typename T>
        FUSE_INLINE void remove_component() {
            _registry->remove_component<T>(_entity);
        }

        template<typename T>
        FUSE_INLINE bool has_component() {
            return _registry->has_component<T>(_entity);
        }

        FUSE_INLINE ecs::entity add_entity() {
            auto entity = _registry->add_entity();
            _registry->add_component<ecs::transform_component>(entity);
            _registry->add_component<ecs::info_component>(entity);
            return ecs::entity(entity, _registry);
        }

    private:
        ecs::entityid _entity = INVALID_ID;
        ecs::registry *_registry = nullptr;
        asset_registry *_assets = nullptr;
    };

    using script_instantiator = std::function<script_instance *(
            const script_props &)>;
}