#pragma once

#include "event/core.h"
#include "event/events.h"
#include "data/data.h"

namespace editor::gui {

    class entity_window : public listener<entity_sel_event> {
    public:
        entity_window(std::shared_ptr<dispatcher>, std::shared_ptr<scene_data>);

        void show();

        void on_event_t(std::shared_ptr<entity_sel_event> e) override;

    private:
        bool _show_entity = false;

        std::shared_ptr<dispatcher> _disp;
        std::shared_ptr<scene_data> _data;

        entity_data _entity_data;
        std::vector<std::function<bool(entity_data &)>> _tabs;
    };
}