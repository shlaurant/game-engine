#include <imgui.h>
#include "entity_window.h"
#include "common/log.h"
#include "event/events.h"

namespace editor::gui {
    entity_window::entity_window(std::shared_ptr<dispatcher> disp,
                                 std::shared_ptr<scene_data> data) :
            _disp(std::move(disp)), _data(std::move(data)) {}

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

    void entity_window::show() {
        bool is_changed = false;

        ImGui::Begin("Entity");

        if (_show_entity) {
            is_changed |= _info_tab.show(_entity_data);
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