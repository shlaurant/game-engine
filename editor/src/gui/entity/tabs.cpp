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

            auto p_tr = entity.comp_data<transform_data>();

            ImGui::PushItemWidth(ItemWidth);
            static char tr_x[BuffSize] = "";
            static char tr_y[BuffSize] = "";
            sprintf(tr_x, "%f", p_tr->translate.x);
            sprintf(tr_y, "%f", p_tr->translate.y);
            ImGui::Text("translation: ");
            ImGui::SameLine(Align);
            if (input_text("x##tr", tr_x, ImGuiInputTextFlags_CharsDecimal)) {
                entity.comp_data<transform_data>()->translate.x = (float) atof(
                        tr_x);
                is_changed = true;
            }
            ImGui::SameLine();
            if (input_text("y##tr", tr_y, ImGuiInputTextFlags_CharsDecimal)) {
                entity.comp_data<transform_data>()->translate.y = (float) atof(
                        tr_y);
                is_changed = true;
            }

            static char rot[BuffSize] = "";
            sprintf(rot, "%f", p_tr->rotation);
            ImGui::Text("rotation: ");
            ImGui::SameLine(Align);
            if (input_text("##rotation", rot,
                           ImGuiInputTextFlags_CharsDecimal)) {
                entity.comp_data<transform_data>()->rotation = (float) atof(
                        rot);
                is_changed = true;
            }

            static char sc_x[BuffSize] = "";
            static char sc_y[BuffSize] = "";
            sprintf(sc_x, "%f", p_tr->scale.x);
            sprintf(sc_y, "%f", p_tr->scale.y);
            ImGui::Text("scale: ");
            ImGui::SameLine(Align);
            if (input_text("x##sc", sc_x, ImGuiInputTextFlags_CharsDecimal)) {
                entity.comp_data<transform_data>()->scale.x = (float) atof(
                        sc_x);
            }
            ImGui::SameLine();
            if (input_text("y##sc", sc_y, ImGuiInputTextFlags_CharsDecimal)) {
                entity.comp_data<transform_data>()->scale.y = (float) atof(
                        sc_y);
            }
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

            INPUT_FLOAT(gravity_scale, gs, rb->gravity_scale, float)

            ImGui::Text("velocity");
            ImGui::SameLine();
            INPUT_FLOAT(x, vx, rb->velocity.x, float)
            ImGui::SameLine();
            INPUT_FLOAT(y, vy, rb->velocity.y, float);

            ImGui::Text("force");
            ImGui::SameLine();
            INPUT_FLOAT(x, fx, rb->force.x, float);
            ImGui::SameLine();
            INPUT_FLOAT(y, fy, rb->force.y, float);

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
