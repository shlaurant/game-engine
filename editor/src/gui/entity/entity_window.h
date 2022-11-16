#pragma once

#include "gui/gui.h"
#include "event/core.h"
#include "event/events.h"
#include "data/scene_data.h"

namespace editor::gui {

    class entity_window : public window {
    public:
        entity_window();

        void show() override;

        void load_entity(entity_data);

    private:
        bool _entity_loaded = false;

        std::shared_ptr<dispatcher> _disp;

        entity_data _entity_data;

        std::vector<std::function<bool(entity_data &)>> _tabs;

        void add_comp_popup(bool &);
    };
}