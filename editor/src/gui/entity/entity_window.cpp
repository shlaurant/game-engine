#include <imgui.h>
#include "entity_window.h"
#include "common/log.h"
#include "event/events.h"
#include "tabs.h"

namespace editor::gui {
    entity_window::entity_window(std::shared_ptr<dispatcher> disp,
                                 std::shared_ptr<scene_data> data) :
            _disp(std::move(disp)), _data(std::move(data)) {
        _tabs.push_back(tab::information);
    }

    void entity_window::show() {
        bool is_changed = false;

        ImGui::Begin("Entity");

        for (auto &e: _tabs) {
            is_changed = e(_entity_data);
        }

        if (is_changed) {
            _data->change_entity(_entity_data);
        }

        ImGui::End();
    }

    void entity_window::on_event_t(std::shared_ptr<entity_sel_event> e) {
        _entity_data = e->data;
        _show_entity = true;
    }
}