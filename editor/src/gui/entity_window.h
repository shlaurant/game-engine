#pragma once

#include "event/core.h"
#include "event/events.h"
#include "data/data.h"

namespace editor::gui {

    class info_tab {
    public:
        bool show(entity_data &);

    private:
        const static int BuffSize = 24;
        char _uuid[BuffSize] = "";
        char _name[BuffSize] = "";
        char _tag[BuffSize] = "";
    };

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
        info_tab _info_tab;
    };
}