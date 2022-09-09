#pragma once

#include "event/core.h"
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

    class entity_window : public listener {
    public:
        entity_window(dispatcher &disp);

        void show();

        void on_event(std::shared_ptr<event> e) override;

    private:
        bool _show_entity = false;
        dispatcher &_disp;
        entity_data _entity_data;

        info_tab _info_tab;
    };
}