#include "gui.h"

namespace editor::gui {
    gui::gui(std::shared_ptr<dispatcher> disp, std::shared_ptr<scene_data> data){
        _p_scene_window = std::make_shared<scene_window>(disp, data);
        _p_entity_window = std::make_shared<entity_window>(disp, data);

        disp->add_listener<entity_sel_event>(_p_entity_window);
    }

    void gui::show(){
        _p_scene_window->show();
        _p_entity_window->show();
    }
}