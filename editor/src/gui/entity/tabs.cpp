#include <imgui.h>
#include "tabs.h"

namespace editor::gui::tab {
    bool information(entity_data &entity) {
        bool is_changed = false;

        strcpy_s(name, entity.name().c_str());
        strcpy_s(tag, entity.info.tag.c_str());

        if (ImGui::TreeNode("information")) {
            ImGui::Text("uuid: %llu", entity.uuid());
            ImGui::AlignTextToFramePadding();
            ImGui::Text("name: ");
            ImGui::SameLine();
            if (ImGui::InputText("##name", name, BuffSize) &&
                ImGui::IsItemDeactivatedAfterEdit()) {
                is_changed = true;
                entity.info.name = std::string(name);
            }
            ImGui::AlignTextToFramePadding();
            ImGui::Text("tag: ");
            ImGui::SameLine();
            ImGui::InputText("##tag", tag, BuffSize);
            ImGui::TreePop();
        }

        return is_changed;
    }
}