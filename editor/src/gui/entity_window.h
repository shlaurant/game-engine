#pragma once

#include "../common/event.h"
#include "../data.h"

namespace editor::gui {
    class entity_window:public listener {
    public:
        entity_window(dispatcher &disp);

        void show();

        void on_event(std::shared_ptr<event> e) override;

    private:
        bool _show_entity = false;
        dispatcher &_disp;
        entity_data _entity_data;
    };
}