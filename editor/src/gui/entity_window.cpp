#include <imgui.h>
#include "entity_window.h"
#include "common/log.h"
#include "event/events.h"

namespace editor::gui {
    bool info_tab::show(entity_data &entity) {
        bool is_changed = false;

        strcpy_s(_name, entity.name().c_str());
        strcpy_s(_tag, entity.info.tag.c_str());

        if (ImGui::TreeNode("information")) {
            ImGui::Text("uuid: %llu", entity.uuid());
            ImGui::AlignTextToFramePadding();
            ImGui::Text("name: ");
            ImGui::SameLine();
            if (ImGui::InputText("##name", _name, BuffSize) &&
                ImGui::IsItemDeactivatedAfterEdit()) {
                is_changed = true;
                entity.info.name = std::string(_name);
            }
            ImGui::AlignTextToFramePadding();
            ImGui::Text("tag: ");
            ImGui::SameLine();
            ImGui::InputText("##tag", _tag, BuffSize);
            ImGui::TreePop();
        }

        return is_changed;
    }

    entity_window::entity_window(dispatcher &disp) : _disp(disp) {
        auto p = std::shared_ptr<listener>(this);
        _disp.add_listener<entity_sel_event>(p);
    }

    void entity_window::show() {
        bool is_changed = false;

        ImGui::Begin("Entity");

        if (_show_entity) {
            is_changed |= _info_tab.show(_entity_data);
        }

        if(is_changed){
            _disp.post<entity_change_event>(_entity_data);
        }

        ImGui::End();
    }

    void entity_window::on_event(std::shared_ptr<event> e) {
        auto p = std::static_pointer_cast<entity_sel_event>(e);
        _entity_data = p->data;
        _show_entity = true;
    }
}