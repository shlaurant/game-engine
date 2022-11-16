#include <imgui.h>
#include "data/physics.h"
#include "data/common.h"
#include "tabs.h"

namespace editor::gui::tab {
    bool information(entity_data &entity) {
        bool is_changed = false;

        static char name[BuffSize] = "";
        static char tag[BuffSize] = "";

        strcpy_s(name, entity.comp_data<info_data>()->name.c_str());
        strcpy_s(tag, entity.comp_data<info_data>()->tag.c_str());

        if (ImGui::TreeNode("information")) {
            ImGui::Text("uuid: %llu", entity.comp_data<info_data>()->uuid);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("name: ");
            ImGui::SameLine();
            if (ImGui::InputText("##name", name, BuffSize) &&
                ImGui::IsItemDeactivatedAfterEdit()) {
                is_changed = true;
                entity.comp_data<info_data>()->name = std::string(name);
            }

            ImGui::AlignTextToFramePadding();
            ImGui::Text("tag: ");
            ImGui::SameLine();
            ImGui::InputText("##tag", tag, BuffSize);
            ImGui::TreePop();
        }

        return is_changed;
    }

    bool transform(entity_data &entity) {
        if (!entity.has_comp<transform_data>()) return false;

        bool is_changed = false;

        if (ImGui::TreeNode("transform")) {
            ImGui::Indent();
            ImGui::PushItemWidth(ItemWidth);

            auto p_tr = entity.comp_data<transform_data>();

            ImGui::Text("translation: ");
            ImGui::SameLine(Align);
            INPUT_FLOAT(x, tr_x, p_tr->translate.x);
            ImGui::SameLine();
            INPUT_FLOAT(y, tr_y, p_tr->translate.y);

            INPUT_FLOAT_A(rotation: , rot, p_tr->rotation, Align)

            ImGui::Text("scale: ");
            ImGui::SameLine(Align);
            INPUT_FLOAT(x, sc_x, p_tr->scale.x)
            ImGui::SameLine();
            INPUT_FLOAT(x, sc_y, p_tr->scale.y)

            ImGui::PopItemWidth();
            ImGui::Unindent();
            ImGui::TreePop();
        }

        return is_changed;
    }

    bool rigidbody(entity_data &entity) {
        if (!entity.has_comp<rigidbody_data>()) return false;
        bool is_changed = false;

        if (ImGui::TreeNode("rigidbody")) {
            ImGui::PushItemWidth(ItemWidth);
            ImGui::Indent();
            auto rb = entity.comp_data<rigidbody_data>();

            if(ImGui::Checkbox("disable", &(rb->disabled)) && ImGui::IsItemDeactivatedAfterEdit()){
                is_changed = true;
            }

            INPUT_FLOAT(gravity_scale, gs, rb->gravity_scale)

            ImGui::Text("velocity");
            ImGui::SameLine();
            INPUT_FLOAT(x, vx, rb->velocity.x)
            ImGui::SameLine();
            INPUT_FLOAT(y, vy, rb->velocity.y);

            ImGui::Text("force");
            ImGui::SameLine();
            INPUT_FLOAT(x, fx, rb->force.x);
            ImGui::SameLine();
            INPUT_FLOAT(y, fy, rb->force.y);

            ImGui::Unindent();
            ImGui::PopItemWidth();
            ImGui::TreePop();
        }

        return is_changed;
    }

    bool input_text(const char *tag, char (&buf)[BuffSize],
                    ImGuiInputTextFlags_ flags = ImGuiInputTextFlags_None) {
        return ImGui::InputText(tag, buf, BuffSize, flags) &&
               ImGui::IsItemDeactivatedAfterEdit();
    }

    float to_float(char *arr) {
        return (float) atof(arr);
    }
}
