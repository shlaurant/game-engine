#include "entity_window.h"
#include "../common/log.h"

namespace editor::gui {
    entity_window::entity_window(dispatcher &disp):_disp(disp) {
        auto p = std::shared_ptr<listener>(this);
        _disp.add_listener<entity_sel_event>(p);
    }

    void entity_window::show() {

    }

    void entity_window::on_event(std::shared_ptr<event> e) {
        log_info("recieved event");
    }
}